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
LiquidCrystal_I2C lcd(0x27, 16, 2); // Back to 16x2

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

// Alarm types
enum AlarmType {
  ALARM_NONE,
  ALARM_EXPLOSION,
  ALARM_DISARM,
  ALARM_SELF_DESTRUCT,
  ALARM_PREMATURE
};
bool force_start_menu_update = false;

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
  Serial.println("Initializing LCD...");
  delay(1000); // Longer delay before init
  lcd.init();
  delay(500); // Longer delay after init
  lcd.backlight();
  delay(100);
  clear_lcd();
  delay(100);
  lcd.home();
  lcd.print("Test LCD");
  delay(100);
  Serial.println("LCD initialized and test text sent");

  // pinout setup
  pinMode(siren_output, OUTPUT);
  digitalWrite(siren_output, LOW);
  // activity led & buzzer
  pinMode(activity_led_output, OUTPUT);
  digitalWrite(activity_led_output, LOW);

  // init variables
  reset_current_password_input_no_clear();
  
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

// Unified display system
struct DisplayCache {
  String lastPasswordDisplay = "";
  String lastTimerDisplay = "";
  bool needsUpdate = false;
  int lastDisplayedSeconds = -1;
} display;

// Unified input system
struct InputSystem {
  unsigned long lastKeyPress = 0;
  char lastKey = '\0';
  const unsigned long DEBOUNCE_DELAY = 150;
} input;

void update_display(bool is_timer = false) {
  if (is_timer) {
    String currentTimerDisplay = String(timer_buf);
    if (currentTimerDisplay != display.lastTimerDisplay || display.needsUpdate) {
      print_line(timer_buf, 1);
      display.lastTimerDisplay = currentTimerDisplay;
      display.needsUpdate = false;
    }
    lcd.setCursor(timer_count, 1);
  } else {
    String currentPasswordDisplay = String(current_password);
    if (currentPasswordDisplay != display.lastPasswordDisplay || display.needsUpdate) {
      print_line(current_password, 1);
      display.lastPasswordDisplay = currentPasswordDisplay;
      display.needsUpdate = false;
    }
    lcd.setCursor(pass_count, 1);
  }
}

void process_input(bool is_timer = false){
  char input_key = keypad.getKey();
  if (input_key) {
    unsigned long currentTime = millis();
    
    // Debounce: ignore if same key pressed within debounce delay
    if (input_key == input.lastKey && (currentTime - input.lastKeyPress) < input.DEBOUNCE_DELAY) {
      return;
    }
    
    input.lastKey = input_key;
    input.lastKeyPress = currentTime;
    
    // Trigger activity LED
    activityActive = false; // Reset to allow new activity
    indicate_activity(activity_led_output);
    handle_key_input(input_key, is_timer);
    display.needsUpdate = true; // Mark for update when key is pressed
  }
  
  update_display(is_timer);
}

void reset_current_timer_input() {
  memset(timer_buf, '\0', (timer_length + 1) * sizeof(char));
  timer_count = 0;
  clear_lcd();
  display.lastTimerDisplay = ""; // Reset cache
  display.needsUpdate = true;
}

void valid_pass(){
  attempts = 0;
}

void print_line(const char* string, uint8_t line) {
  lcd.setCursor(0, line);
  lcd.printstr(string);
  Serial.print("LCD Line "); Serial.print(line); Serial.print(": "); Serial.println(string);
}

void print_digit(char value, uint8_t line, uint8_t pos) {
  lcd.setCursor(pos, line);
  lcd.print(value);
  Serial.print("LCD Digit at ("); Serial.print(pos); Serial.print(","); Serial.print(line); Serial.print("): "); Serial.println(value);
}

void clear_lcd() {
  lcd.clear();
  Serial.println("LCD: CLEARED");
}

void format_sec_to_print(int sec){
  // Only update display if seconds have changed
  if (sec != display.lastDisplayedSeconds) {
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
    
    display.lastDisplayedSeconds = sec;
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
  clear_lcd();
  display.lastPasswordDisplay = ""; // Reset cache
  display.needsUpdate = true;
}

void reset_current_password_input_no_clear() {
  memset(current_password, '\0', (Password_Length + 1) * sizeof(char));
  pass_count = 0;
  display.lastPasswordDisplay = ""; // Reset cache
  display.needsUpdate = true;
}

// Unified error system
struct ErrorSystem {
  unsigned long startTime = 0;
  bool active = false;
  int blinkCount = 0;
  unsigned long lastBlink = 0;
  bool blinkState = false;
} error;

void invalid_pass() {
  // print error
  clear_lcd();
  print_line("Senha invalida!", 0);
  error.startTime = millis();
  error.active = true;
  error.blinkCount = 0;
  error.lastBlink = 0;
  error.blinkState = false;
  
  // process error
  attempts++;

  if(attempts >= max_attempts) {
    if(current_state == INIT_STATE) {
      process_self_destruct();
    } else if(current_state == CD_STATE) {
      process_premature_explosion();
    }
    attempts = 0;
    error.active = false;
  }
}

void handle_error_blinking() {
  if (error.active && attempts < max_attempts) {
    unsigned long currentTime = millis();
    
    if (currentTime - error.lastBlink >= (error.blinkState ? 100 : 200)) {
      digitalWrite(activity_led_output, error.blinkState ? LOW : HIGH);
      error.blinkState = !error.blinkState;
      error.lastBlink = currentTime;
      
      if (!error.blinkState) {
        error.blinkCount++;
        if (error.blinkCount >= (max_attempts - attempts) * 2) {
          error.active = false;
          digitalWrite(activity_led_output, LOW);
        }
      }
    }
  }
}

void validate_pass() {
  Serial.print("Validating password: ");
  Serial.println(current_password);
  
  // Check master password
  if (strcmp(MASTER_PASSWORD, current_password) == 0) {
    handle_master_password();
  } else {
    // Check team passwords
    int team_index = find_team_password();
    if (team_index >= 0) {
      handle_team_password(team_index);
    } else {
      Serial.println("Invalid password!");
      invalid_pass();
    }
  }
  reset_current_password_input();
}

int find_team_password() {
  for (int i = 0; i < 3; i++) {
    if (strcmp(valid_passwords[i], current_password) == 0) {
      return i;
    }
  }
  return -1;
}

void handle_master_password() {
  valid_pass();
  if (current_state == INIT_STATE) {
    Serial.println("Master password correct - entering admin mode");
    current_state = ADM_STATE;
  } else {
    Serial.println("Master password correct - resetting to init state");
    reset_start_menu_state();
  }
}

void handle_team_password(int team_index) {
  Serial.print("Team password correct - Team ");
  Serial.println((char)('A' + team_index));
  valid_pass();
  pass_team = 'A' + team_index;
  
  if (current_state == INIT_STATE) {
    Serial.println("Starting countdown...");
    start_countdown();
  } else if (current_state == CD_STATE) {
    Serial.println("Bomb disarmed!");
    process_disarm_bomb();
  }
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
            // Force LCD update by resetting the static variables in start_menu
            reset_start_menu_state();
            // Reset admin options
            reset_admin_options();
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

// Unified alarm system
struct AlarmSystem {
  AlarmType type = ALARM_NONE;
  unsigned long startTime = 0;
  int count = 0;
  bool active = false;
  unsigned long lastSiren = 0;
  bool sirenState = false;
} alarm;

void start_alarm(AlarmType type) {
  alarm.type = type;
  alarm.startTime = millis();
  alarm.count = 0;
  alarm.active = true;
  alarm.lastSiren = 0;
  alarm.sirenState = false;
}

void process_bomb_exploded(){
  start_alarm(ALARM_EXPLOSION);
  current_state = EXPLOSION_STATE;
  clear_lcd();
}

void process_disarm_bomb(){
  start_alarm(ALARM_DISARM);
  current_state = DISARMED_STATE;
  clear_lcd();
}

void process_self_destruct(){
  clear_lcd();
  print_line("Missao falha!", 0);
  print_line("Todo time morto", 1);
  start_alarm(ALARM_SELF_DESTRUCT);
}

void process_premature_explosion() {
  clear_lcd();
  start_alarm(ALARM_PREMATURE);
  current_state = PREMATURE_EXPLOSION_STATE;
}

void handle_alarm() {
  if (!alarm.active) return;
  
  unsigned long currentTime = millis();
  bool shouldSiren = false;
  int sirenDuration, pauseDuration;
  
  switch (alarm.type) {
    case ALARM_EXPLOSION:
      if (currentTime - alarm.startTime < 5000) {
        shouldSiren = true;
        sirenDuration = 5000;
        pauseDuration = 0;
      } else {
        alarm.active = false;
        digitalWrite(siren_output, LOW);
        return;
      }
      break;
      
    case ALARM_DISARM:
      if (currentTime - alarm.startTime < 1000) {
        shouldSiren = true;
        sirenDuration = 1000;
        pauseDuration = 0;
      } else {
        alarm.active = false;
        digitalWrite(siren_output, LOW);
        return;
      }
      break;
      
    case ALARM_SELF_DESTRUCT:
      sirenDuration = 1000;
      pauseDuration = 100;
      if (alarm.count < 3) {
        shouldSiren = true;
      } else {
        alarm.active = false;
        digitalWrite(siren_output, LOW);
        clear_lcd();
        return;
      }
      break;
      
    case ALARM_PREMATURE:
      sirenDuration = (alarm.count < 2) ? 1000 : 3000;
      pauseDuration = 100;
      if (alarm.count < 3) {
        shouldSiren = true;
      } else {
        alarm.active = false;
        digitalWrite(siren_output, LOW);
        return;
      }
      break;
      
    default:
      alarm.active = false;
      return;
  }
  
  if (shouldSiren) {
    int cycleTime = alarm.sirenState ? pauseDuration : sirenDuration;
    if (currentTime - alarm.lastSiren >= cycleTime) {
      digitalWrite(siren_output, alarm.sirenState ? LOW : HIGH);
      alarm.sirenState = !alarm.sirenState;
      alarm.lastSiren = currentTime;
      
      if (!alarm.sirenState) {
        alarm.count++;
      }
    }
  }
}

// journey orchestration functions

// Function to reset start_menu state variables
void reset_start_menu_state() {
  current_state = INIT_STATE;
  // We can't directly access static variables, so we'll use a global flag
  force_start_menu_update = true;
}

// Function to reset admin options
void reset_admin_options() {
  adm_opt = adm_opt_default;
  Serial.println("Admin options reset to default");
}

void start_menu() {
  static bool firstTime = true;
  static int lastState = -1;
  
  // Check if we're entering this state from another state or if forced update
  if (firstTime || lastState != INIT_STATE || force_start_menu_update) {
    Serial.println("start_menu: Display update needed");
    clear_lcd();
    delay(10); // Small delay to ensure clear completes
    print_line("Insira a senha:", 0);
    display.needsUpdate = true; // Force display update
    firstTime = false;
    lastState = INIT_STATE;
    force_start_menu_update = false; // Reset the flag
    Serial.println("Displaying: Insira a senha:");
  }
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

void show_status_menu(const char* message) {
  print_line(message, 0);
  print_digit(pass_team, 0, 14);
  process_input();
}

void exploded_menu(){
  show_status_menu("BOOOM! Senha:");
}

void premature_explosion_menu(){
  show_status_menu("Ooops! Senha:");
}

void disarmed_menu(){
  show_status_menu("Desarmada!");
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
        reset_current_password_input();
      } else if(input_key == '#') {
        // Exit admin mode and return to init state
        reset_current_password_input();
        reset_start_menu_state();
        Serial.println("Exiting admin mode - returning to init state");
      } else {
        adm_opt = adm_opt_default;
      }
      clear_lcd();
    }
  } else if (adm_opt == adm_opt_timer) {
    print_line("Enter minutes:", 0);
    process_input(true);
  } else if (adm_opt == adm_opt_password) {
    handle_admin_password_input();
  } else {
    reset_current_password_input();
  }
}

void handle_admin_password_input() {
  print_line("Insert pass", 0);
  
  char input_key = keypad.getKey();
  if (input_key) {
    // Use unified input system for debouncing
    unsigned long currentTime = millis();
    
    if (input_key == input.lastKey && (currentTime - input.lastKeyPress) < input.DEBOUNCE_DELAY) {
      return;
    }
    
    input.lastKey = input_key;
    input.lastKeyPress = currentTime;
    
    // Trigger activity LED
    activityActive = false;
    indicate_activity(activity_led_output);
    
    if (input_key >= '0' && input_key <= '9' || input_key == '*') {
      // Collect password digits
      if (pass_count < Password_Length) {
        current_password[pass_count] = input_key;
        pass_count++;
        print_line(current_password, 1);
        lcd.setCursor(pass_count, 1);
      }
    } else if (input_key == 'A' || input_key == 'B' || input_key == 'C') {
      // Team selection - save password
      if (pass_count == Password_Length) {
        int team_index = input_key - 'A';
        strcpy(valid_passwords[team_index], current_password);
        
        Serial.print("Password updated for Team ");
        Serial.print(input_key);
        Serial.print(": ");
        Serial.println(valid_passwords[team_index]);
        
        // Reset and return to admin menu
        reset_current_password_input();
        adm_opt = adm_opt_default;
        clear_lcd();
        print_line("Password updated!", 0);
        delay(2000);
      }
    } else if (input_key == 'D') {
      // Cancel
      reset_current_password_input();
      adm_opt = adm_opt_default;
      clear_lcd();
    }
  }
}

void handle_activity_led_buzzer() {
  if (activityActive) {
    if (millis() - activityStartTime >= 50) {
      digitalWrite(activity_led_output, LOW);
      activityActive = false;
    }
  }
}

void loop() {
  // Reset watchdog timer at start of each loop
  wdt_reset();
  
  // Simple heartbeat to show Arduino is running
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 5000) {
    Serial.println("Arduino running...");
    lastHeartbeat = millis();
  }
  
  // Handle non-blocking operations
  handle_activity_led_buzzer(); // Handle activity LED/buzzer
  handle_alarm(); // Handle all alarm types
  handle_error_blinking();
  
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
