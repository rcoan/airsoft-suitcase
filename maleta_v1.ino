//Dependencies
#include <Wire.h>               // default lib for i2c display
#include <LiquidCrystal_I2C.h>  // custom lib to deal with lcd i2c
#include <Keypad.h>
#include <CountDown.h>

//System Definitions
const byte NUMPAD_ROW_QTY = 4;
const byte NUMPAD_COLUMN_QTY = 4;

// Numpad Configs
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
LiquidCrystal_I2C lcd(0x27, 16, 2);

// PINOUT
int siren_output = 12;
int activity_led_output = 13;

// application configs
const char* INIT_STATE = "init_st";
const char* CD_STATE = "cd_st";
const char* PREMATURE_EXPLOSION_STATE = "pe_st";
const char* EXPLOSION_STATE = "exploded_st";
const char* DISARMED_STATE = "disarmed_st";
const char* ADM_STATE = "adm_st";
char* state = INIT_STATE;

// Timer
int CD_TIME_M = 1;
int last_second = 0;
CountDown timer;
char display_timer_value[16];

// password related
#define Password_Length 7

char MASTER_PASSWORD[Password_Length] = "*5456*";
char valid_passwords[3][Password_Length] = {
  "123456",
  "987654",
  "741852"
};
char cr_pass[Password_Length];
char input_key;
byte pass_count = 0;
int attempts = 0;
const int max_attempts = 3;
char pass_team = 'N';

void setup() {
  // serial for debug
  Serial.begin(9600);
  
  // LCD setup
  lcd.backlight();
  lcd.init();

  // pinout setup
  pinMode(siren_output, OUTPUT);
  digitalWrite(siren_output, LOW);
  pinMode(activity_led_output, OUTPUT);
  digitalWrite(activity_led_output, LOW);

  // init variables
  reset_cr_pass_input();
}
// Utils
void process_input(){
  process_key_input();
  print_line(cr_pass, 1);
  lcd.setCursor(pass_count, 1);
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
  int minutes = sec/60;
  int sec_left = sec-(minutes*60);
  String start_txt = String("Ativacao: ");
  String minutes_only;
  if(minutes < 10) {
    minutes_only = String(start_txt + String("0") + minutes);
  } else {
    minutes_only = String(start_txt + minutes);
  }
  String minutes_only_formatted = String(minutes_only + ":");
  String minutes_sec;
  if(sec_left < 10) {
    minutes_sec = String(minutes_only_formatted + "0" + sec_left);
  } else {
    minutes_sec = String(minutes_only_formatted + sec_left);
  }
  int size = minutes_sec.length() + 1;
  minutes_sec.toCharArray(display_timer_value, size);
  Serial.println(display_timer_value);
}

// countdown
void start_countdown(){
  state = CD_STATE;
  timer.start(0, 0, CD_TIME_M, 0);
}

// Password functions
void reset_cr_pass_input() {
  memset(cr_pass, '\0', Password_Length * sizeof(char));
  pass_count = 0;
  lcd.clear();
}

void invalid_pass() {
  // print error
  lcd.clear();
  print_line("Senha invalida!", 0);
  indicate_activity(activity_led_output, 2000);
  lcd.clear();
  // proccess error
  attempts++;

  if(attempts >= max_attempts) {
    if(state == INIT_STATE) {
      process_self_destruct();
    } else if(state == CD_STATE) {
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
  if (!strcmp(MASTER_PASSWORD, cr_pass)) {
    valid_pass();
    if (state == INIT_STATE){
      state = ADM_STATE;
    } else {
      state = INIT_STATE; 
    }
  } else if (!strcmp(valid_passwords[0], cr_pass)) {
    valid_pass();
    pass_team = 'A';
    if (state == INIT_STATE) {
      start_countdown();
    } else if(state == CD_STATE) {
      process_disarm_bomb();
    }
  } else if (!strcmp(valid_passwords[1], cr_pass)) {
    valid_pass();
    pass_team = 'B';
    if (state == INIT_STATE) {
      start_countdown();
    } else if(state == CD_STATE) {
      process_disarm_bomb();
    }
  } else if (!strcmp(valid_passwords[2], cr_pass)) {
    valid_pass();
    pass_team = 'C';
    if (state == INIT_STATE) {
      start_countdown();
    } else if(state == CD_STATE) {
      process_disarm_bomb();
    }
  } else {
    invalid_pass();
  }
  reset_cr_pass_input();
}

// Proccess input functions
void process_key_input() {
  input_key = keypad.getKey();

  if (input_key) {
    indicate_activity(activity_led_output);
    switch (input_key) {
      case '#':
        validate_pass();
        break;
      case 'A':
        //
        break;
      case 'B':
        //
        break;
      case 'C':
        break;
      case 'D':
        reset_cr_pass_input();
        break;
      default:
        // add input to array
        cr_pass[pass_count] = input_key;
        pass_count++;
        if (pass_count == (Password_Length -1)) {
          validate_pass();
        }
        break;
    }
  }
}

// Process end states

void process_bomb_exploded(){
  indicate_activity(siren_output, 5000);
  state = EXPLOSION_STATE;
  lcd.clear();
}

void process_disarm_bomb(){
  indicate_activity(siren_output, 1000);
  state = DISARMED_STATE;
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
  state = PREMATURE_EXPLOSION_STATE;
}

// journey orchestration functions

void start_menu() {
  print_line("Insira a senha:", 0);
  process_input();
}

void countdown_menu(){
  int seconds_left = timer.remaining();
  format_sec_to_print(seconds_left);
  print_line(display_timer_value, 0);
  process_input();
  
  if(last_second != seconds_left) {
    indicate_activity(activity_led_output);
    last_second = seconds_left;
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

void loop() {
  if (state == INIT_STATE) {
    start_menu();
  } else if (state == CD_STATE){
    countdown_menu();
  } else if (state == EXPLOSION_STATE) {
    exploded_menu();
  } else if (state == PREMATURE_EXPLOSION_STATE) {
    premature_explosion_menu();
  } else if (state == DISARMED_STATE) {
    disarmed_menu();
  } else {
    print_line(state, 0);
    print_digit(pass_team,1, 0);
  }
}
