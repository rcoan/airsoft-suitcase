//Dependencies
#include <Wire.h>               // default lib for i2c display
#include <LiquidCrystal_I2C.h>  // custom lib to deal with lcd i2c
#include <Keypad.h>             // Lib to handle numpad inputs
#include <CountDown.h>          // Lib to handle timer
#include <avr/wdt.h>            // Watchdog timer

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
  // Enable watchdog timer (8 second timeout)
  wdt_enable(WDTO_8S);
  
  // serial for debug
  Serial.begin(9600);
  Serial.println("Airsoft Bomb System Starting...");

  // LCD setup
  lcd.backlight();
  lcd.init();
  Serial.println("LCD initialized");

  // pinout setup
  pinMode(siren_output, OUTPUT);
  digitalWrite(siren_output, LOW);
  // activity led & buzzer
  pinMode(activity_led_output, OUTPUT);
  digitalWrite(activity_led_output, LOW);

  // init variables
  reset_current_password_input();
  
  // Reset watchdog after successful setup
  wdt_reset();
}

// Utils
unsigned long activityStartTime = 0;
bool activityActive = false;

void indicate_activity(int pin, int duration = 50){
  if (!activityActive) {
    digitalWrite(pin, HIGH);
    activityStartTime = millis();
    activityActive = true;
  } else if (millis() - activityStartTime >= duration) {
    digitalWrite(pin, LOW);
    activityActive = false;
  }
}

// LCD caching variables
static String lastPasswordDisplay = "";
static String lastTimerDisplay = "";
static bool lcdNeedsUpdate = false;

// Input debouncing
static unsigned long lastKeyPress = 0;
static char lastKey = '\0';
const unsigned long DEBOUNCE_DELAY = 150; // ms

void process_input(bool is_timer = false){
  char input_key = keypad.getKey();
  if (input_key) {
    unsigned long currentTime = millis();
    
    // Debounce: ignore if same key pressed within debounce delay
    if (input_key == lastKey && (currentTime - lastKeyPress) < DEBOUNCE_DELAY) {
      return;
    }
    
    lastKey = input_key;
    lastKeyPress = currentTime;
    
    indicate_activity(activity_led_output);
    handle_key_input(input_key, is_timer);
    lcdNeedsUpdate = true; // Mark for update when key is pressed
  }
  
  if (is_timer) {
    String currentTimerDisplay = String(timer_buf);
    if (currentTimerDisplay != lastTimerDisplay || lcdNeedsUpdate) {
      print_line(timer_buf, 1);
      lastTimerDisplay = currentTimerDisplay;
      lcdNeedsUpdate = false;
    }
    lcd.setCursor(timer_count, 1);
  } else {
    String currentPasswordDisplay = String(current_password);
    if (currentPasswordDisplay != lastPasswordDisplay || lcdNeedsUpdate) {
      print_line(current_password, 1);
      lastPasswordDisplay = currentPasswordDisplay;
      lcdNeedsUpdate = false;
    }
    lcd.setCursor(pass_count, 1);
  }
}

void reset_current_timer_input() {
  memset(timer_buf, '\0', (timer_length + 1) * sizeof(char));
  timer_count = 0;
  lcd.clear();
  lastTimerDisplay = ""; // Reset cache
  lcdNeedsUpdate = true;
}

void valid_pass(){
  attempts = 0;
}

void print_line(const char* string, uint8_t line) {
  lcd.setCursor(0, line);
  lcd.printstr(string);
}

void print_digit(char value, uint8_t line, uint8_t pos) {
  lcd.setCursor(pos, line);
  lcd.print(value);
}

static int lastDisplayedSeconds = -1;

void format_sec_to_print(int sec){
  // Only update display if seconds have changed
  if (sec != lastDisplayedSeconds) {
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
    
    lastDisplayedSeconds = sec;
  }
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
  lastPasswordDisplay = ""; // Reset cache
  lcdNeedsUpdate = true;
}

unsigned long errorStartTime = 0;
bool errorActive = false;
int errorBlinkCount = 0;

void invalid_pass() {
  // print error
  lcd.clear();
  print_line("Senha invalida!", 0);
  errorStartTime = millis();
  errorActive = true;
  errorBlinkCount = 0;
  // process error
  attempts++;

  if(attempts >= max_attempts) {
    if(current_state == INIT_STATE) {
      process_self_destruct();
    } else if(current_state == CD_STATE) {
      process_premature_explosion();
    }
    attempts = 0;
    errorActive = false;
  }
}

void handle_error_blinking() {
  if (errorActive && attempts < max_attempts) {
    unsigned long currentTime = millis();
    static unsigned long lastBlink = 0;
    static bool blinkState = false;
    
    if (currentTime - lastBlink >= (blinkState ? 100 : 200)) {
      digitalWrite(activity_led_output, blinkState ? LOW : HIGH);
      blinkState = !blinkState;
      lastBlink = currentTime;
      
      if (!blinkState) {
        errorBlinkCount++;
        if (errorBlinkCount >= (max_attempts - attempts) * 2) {
          errorActive = false;
          digitalWrite(activity_led_output, LOW);
        }
      }
    }
  }
}

void validate_pass() {
  Serial.print("Validating password: ");
  Serial.println(current_password);
  if (strcmp(MASTER_PASSWORD, current_password) == 0) {
    Serial.println("Master password correct - entering admin mode");
    valid_pass();
    current_state = (current_state == INIT_STATE) ? ADM_STATE : INIT_STATE;
  } else {
    bool found = false;
    for (int i = 0; i < 3; i++) {
      if (strcmp(valid_passwords[i], current_password) == 0) {
        Serial.print("Team password correct - Team ");
        Serial.println((char)('A' + i));
        valid_pass();
        pass_team = 'A' + i;
        if (current_state == INIT_STATE) {
          Serial.println("Starting countdown...");
          start_countdown();
        } else if (current_state == CD_STATE) {
          Serial.println("Bomb disarmed!");
          process_disarm_bomb();
        }
        found = true;
        break;
      }
    }
    if (!found) {
      Serial.println("Invalid password!");
      invalid_pass();
    }
  }
  reset_current_password_input();
}

// Process input functions
void handle_key_input(char input_key, bool is_timer) {
  Serial.print("Key pressed: ");
  Serial.println(input_key);
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

unsigned long selfDestructStartTime = 0;
int selfDestructSirenCount = 0;
bool selfDestructActive = false;

void process_self_destruct(){
  lcd.clear();
  print_line("Missao falha!", 0);
  print_line("Todo time morto", 1);
  selfDestructStartTime = millis();
  selfDestructSirenCount = 0;
  selfDestructActive = true;
}

void handle_self_destruct() {
  if (selfDestructActive) {
    unsigned long currentTime = millis();
    static unsigned long lastSiren = 0;
    static bool sirenState = false;
    
    if (currentTime - lastSiren >= (sirenState ? 100 : 1000)) {
      digitalWrite(siren_output, sirenState ? LOW : HIGH);
      sirenState = !sirenState;
      lastSiren = currentTime;
      
      if (!sirenState) {
        selfDestructSirenCount++;
        if (selfDestructSirenCount >= 3) {
          selfDestructActive = false;
          digitalWrite(siren_output, LOW);
          lcd.clear();
        }
      }
    }
  }
}

unsigned long prematureStartTime = 0;
int prematureSirenCount = 0;
bool prematureActive = false;

void process_premature_explosion() {
  lcd.clear();
  prematureStartTime = millis();
  prematureSirenCount = 0;
  prematureActive = true;
  current_state = PREMATURE_EXPLOSION_STATE;
}

void handle_premature_explosion() {
  if (prematureActive) {
    unsigned long currentTime = millis();
    static unsigned long lastSiren = 0;
    static bool sirenState = false;
    
    int sirenDuration = (prematureSirenCount < 2) ? 1000 : 3000;
    int pauseDuration = 100;
    
    if (currentTime - lastSiren >= (sirenState ? pauseDuration : sirenDuration)) {
      digitalWrite(siren_output, sirenState ? LOW : HIGH);
      sirenState = !sirenState;
      lastSiren = currentTime;
      
      if (!sirenState) {
        prematureSirenCount++;
        if (prematureSirenCount >= 3) {
          prematureActive = false;
          digitalWrite(siren_output, LOW);
        }
      }
    }
  }
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
  // Reset watchdog timer at start of each loop
  wdt_reset();
  
  // Handle non-blocking operations
  handle_error_blinking();
  handle_self_destruct();
  handle_premature_explosion();
  
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
