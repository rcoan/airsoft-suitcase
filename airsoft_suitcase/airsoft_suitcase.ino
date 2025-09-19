//Dependencies
#include <Wire.h>               // default lib for i2c display
#include <LiquidCrystal_I2C.h>  // custom lib to deal with lcd i2c
#include <Keypad.h>             // Lib to handle numpad inputs
#include <CountDown.h>          // Lib to handle timer

// System Definitions

// Numpad Configs
const byte NUMPAD_ROW_QTY = 4;
const byte NUMPAD_COLUMN_QTY = 4;
const char NUMPAD_KEY_VALUES[NUMPAD_ROW_QTY][NUMPAD_COLUMN_QTY] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

const byte ROWS_PINS[NUMPAD_ROW_QTY] = { 9, 8, 7, 6 };
const byte COLUMNS_PINS[NUMPAD_ROW_QTY] = { 5, 4, 3, 2 };

Keypad keypad = Keypad(makeKeymap(NUMPAD_KEY_VALUES),
                       ROWS_PINS,
                       COLUMNS_PINS,
                       NUMPAD_ROW_QTY,
                       NUMPAD_COLUMN_QTY);

// LCD display configs
LiquidCrystal_I2C lcd(0x27, 16, 2); // First param depends on the lcd model. Check yours.

// Timer
#define timer_length 2 // Max size for 2-digit input

int CD_TIME_M = 10; // default value for countdown in minutes
char timer_buf[timer_length + 1]; // +1 for null terminator
byte timer_count = 0;
int last_second_counted = 0; // def last second processed
CountDown timer; // define timer object
char display_timer_value[16]; // define countdown timer display string to be modified

// password related
#define Password_Length 6 // Max size for 6-digit passwords

char MASTER_PASSWORD[Password_Length + 1] = "*5456*";
char valid_passwords[3][Password_Length + 1] = {
  "123456", // Team A
  "987654", // Team B
  "741852"  // Team C
};

char current_password[Password_Length + 1]; // current password being entered
byte pass_count = 0; // amount of digits in the current password array
int attempts = 0; // failed attempts since last reset
const int max_attempts = 3; // maximum of attempts before action required
char pass_team = 'M'; // default password team

// PINOUT
int siren_output = 12;
int activity_led_output = 13;

// App states configs
enum State {
  INIT_STATE,
  CD_STATE,
  PREMATURE_EXPLOSION_STATE,
  EXPLOSION_STATE,
  DISARMED_STATE,
  ADM_STATE
};
State current_state = INIT_STATE;

const char adm_opt_timer = 'T'; // alterar minutos do timer
const char adm_opt_password = 'P'; // alterar senha. A = team A, B = Team B, C = Team C, 0 = Master
const char adm_opt_default = 'D';

char adm_opt = adm_opt_default; // adm action selected

void setup() {
  // serial for debug
  Serial.begin(9600);

  // LCD setup
  lcd.backlight();
  lcd.init();

  // pinout setup
  pinMode(siren_output, OUTPUT);
  digitalWrite(siren_output, LOW);
  // activity led & buzzer
  pinMode(activity_led_output, OUTPUT);
  digitalWrite(activity_led_output, LOW);

  // init variables
  reset_current_password_input();
}

// Utils
void process_input(bool is_timer = false){
  char input_key = keypad.getKey();
  if (input_key) {
    indicate_activity(activity_led_output);
    handle_key_input(input_key, is_timer);
  }
  
  if (is_timer) {
    print_line(timer_buf, 1);
    lcd.setCursor(timer_count, 1);
  } else {
    print_line(current_password, 1);
    lcd.setCursor(pass_count, 1);
  }
}

void reset_current_timer_input() {
  memset(timer_buf, '\0', (timer_length + 1) * sizeof(char));
  timer_count = 0;
  lcd.clear();
}

void indicate_activity(int pin, int duration = 50){
  digitalWrite(pin, HIGH);
  delay(duration);
  digitalWrite(pin, LOW);
}

void valid_pass(){
  attempts = 0;
}

void print_line(char* string, uint8_t line) {
  lcd.setCursor(0, line);
  lcd.printstr(string);
}

void print_digit(char value, uint8_t line, uint8_t pos) {
  lcd.setCursor(pos, line);
  lcd.print(value);
}

void format_sec_to_print(int sec){
  int minutes = sec / 60;
  int sec_left = sec % 60;
  
  lcd.setCursor(0, 0);
  lcd.print("Ativacao: ");
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if (sec_left < 10) lcd.print("0");
  lcd.print(sec_left);
  
  Serial.print("Ativacao: ");
  if (minutes < 10) Serial.print("0");
  Serial.print(minutes);
  Serial.print(":");
  if (sec_left < 10) Serial.print("0");
  Serial.println(sec_left);
}

// countdown
void start_countdown(){
  current_state = CD_STATE;
  timer.start(0, 0, CD_TIME_M, 0);
}

// Password functions
void reset_current_password_input() {
  memset(current_password, '\0', (Password_Length + 1) * sizeof(char));
  pass_count = 0;
  lcd.clear();
}

void invalid_pass() {
  // print error
  lcd.clear();
  print_line("Senha invalida!", 0);
  indicate_activity(activity_led_output, 2000);
  lcd.clear();
  // process error
  attempts++;

  if(attempts >= max_attempts) {
    if(current_state == INIT_STATE) {
      process_self_destruct();
    } else if(current_state == CD_STATE) {
      process_premature_explosion();
    }
    attempts = 0;
  } else {
    // blink number of attempts
    for(int i = 0; i < max_attempts-attempts; i++){
      delay(200);
      indicate_activity(activity_led_output);
      delay(100);
    }
  }
}

void validate_pass() {
  if (strcmp(MASTER_PASSWORD, current_password) == 0) {
    valid_pass();
    current_state = (current_state == INIT_STATE) ? ADM_STATE : INIT_STATE;
  } else {
    bool found = false;
    for (int i = 0; i < 3; i++) {
      if (strcmp(valid_passwords[i], current_password) == 0) {
        valid_pass();
        pass_team = 'A' + i;
        if (current_state == INIT_STATE) {
          start_countdown();
        } else if (current_state == CD_STATE) {
          process_disarm_bomb();
        }
        found = true;
        break;
      }
    }
    if (!found) {
      invalid_pass();
    }
  }
  reset_current_password_input();
}

// Process input functions
void handle_key_input(char input_key, bool is_timer) {
  if (is_timer) {
    switch (input_key) {
      case 'D':
        reset_current_timer_input();
        break;
      default:
        if (input_key >= '0' && input_key <= '9') {
          timer_buf[timer_count] = input_key;
          timer_count++;
          if (timer_count == timer_length) {
            CD_TIME_M = (timer_buf[0] - '0') * 10 + (timer_buf[1] - '0');
            reset_current_timer_input();
            current_state = INIT_STATE;
          }
        }
        break;
    }
  } else {
    switch (input_key) {
      case '#':
        validate_pass();
        break;
      case 'D':
        reset_current_password_input();
        break;
      default:
        if (input_key >= '0' && input_key <= '9' || input_key == '*') {
          current_password[pass_count] = input_key;
          pass_count++;
          if (pass_count == Password_Length) {
            validate_pass();
          }
        }
        break;
    }
  }
}

// Process end states

void process_bomb_exploded(){
  indicate_activity(siren_output, 5000);
  current_state = EXPLOSION_STATE;
  lcd.clear();
}

void process_disarm_bomb(){
  indicate_activity(siren_output, 1000);
  current_state = DISARMED_STATE;
  lcd.clear();
}

void process_self_destruct(){
  lcd.clear();
  print_line("Missao falha!", 0);
  print_line("Todo time morto", 1);
  indicate_activity(siren_output, 1000);
  delay(100);
  indicate_activity(siren_output, 1000);
  delay(100);
  indicate_activity(siren_output, 1000);
  delay(100);
  lcd.clear();
}

void process_premature_explosion() {
  lcd.clear();
  indicate_activity(siren_output, 1000);
  delay(100);
  indicate_activity(siren_output, 1000);
  delay(100);
  indicate_activity(siren_output, 3000);
  current_state = PREMATURE_EXPLOSION_STATE;
}

// journey orchestration functions

void start_menu() {
  print_line("Insira a senha:", 0);
  process_input();
}

void countdown_menu(){
  int seconds_left = timer.remaining();
  format_sec_to_print(seconds_left);
  process_input();

  if(last_second_counted != seconds_left) {
    indicate_activity(activity_led_output);
    last_second_counted = seconds_left;
  }

  if(seconds_left == 0) {
    process_bomb_exploded();
  }
}

void exploded_menu(){
  print_line("BOOOM! Senha:", 0);
  print_digit(pass_team, 0, 14);
  process_input();
}


void premature_explosion_menu(){
  print_line("Ooops! Senha:", 0);
  print_digit(pass_team, 0, 14);
  process_input();
}

void disarmed_menu(){
  print_line("Desarmada!", 0);
  print_digit(pass_team, 0, 14);
  process_input();
}

void admin_menu() {
  if(adm_opt == adm_opt_default) {
    print_line("Admin menu", 0);
    print_line("1=Timer, 2=Pass", 1);
    char input_key = keypad.getKey();

    if (input_key){
      if (input_key == '1') {
        adm_opt = adm_opt_timer;
      } else if(input_key == '2') {
        adm_opt = adm_opt_password;
      } else {
        adm_opt = adm_opt_default;
      }
      lcd.clear();
    }
  } else if (adm_opt == adm_opt_timer) {
    print_line("Enter minutes:", 0);
    process_input(true);
  } else if (adm_opt == adm_opt_password) {
    process_input();
  } else {
    reset_current_password_input();
  }
}

void loop() {
  switch (current_state) {
    case INIT_STATE:
      start_menu();
      break;
    case CD_STATE:
      countdown_menu();
      break;
    case EXPLOSION_STATE:
      exploded_menu();
      break;
    case PREMATURE_EXPLOSION_STATE:
      premature_explosion_menu();
      break;
    case DISARMED_STATE:
      disarmed_menu();
      break;
    case ADM_STATE:
      admin_menu();
      break;
    default:
      print_line("Reset system!", 0);
      break;
  }
}
