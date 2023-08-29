//Dependencies
#include <Wire.h>               // default lib for i2c display
#include <LiquidCrystal_I2C.h>  // custom lib to deal with lcd i2c
#include <Keypad.h>

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
int siren_output = 13;
int activity_led_output = 13;

// application configs
const char* INIT_STATE = "init_st";
const char* CD_STATE = "cd_st";
const char* PREMATURE_EXPLOSION_STATE = "pe_st";
const char* EXPLOSION_STATE = "exploded_st";
const char* ADM_STATE = "adm_st";
char* state = INIT_STATE;

int CD_TIME = 10;

// password related
#define Password_Length 7

char MASTER_PASSWORD[Password_Length] = "000000";
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

  // init variables
  reset_cr_pass_input();
}
// Utils
void blink_light(int pin){
  digitalWrite(pin, HIGH);
  delay(50);
  digitalWrite(pin, LOW);
}

void print_line(char* string, uint8_t line) {
  lcd.setCursor(0, line);
  lcd.printstr(string);
}

void print_digit(char value, uint8_t line, uint8_t pos) {
  lcd.setCursor(pos, line);
  lcd.print(value);
}

// countdown

void start_countdow(){

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
  delay(2000);
  lcd.clear();
  
  // proccess error
  attempts++;
  
  // blink number of attempts
  for(int i = 0; i < max_attempts-attempts; i++){
    blink_light(activity_led_output);
    delay(100);
  }
}

void validate_pass() {
  if (!strcmp(MASTER_PASSWORD, cr_pass)) {
    state = ADM_STATE;
  } else if (!strcmp(valid_passwords[0], cr_pass)) {
    state = CD_STATE;
    pass_team = 'A';
    start_countdow();
  } else if (!strcmp(valid_passwords[1], cr_pass)) {
    state = CD_STATE;
    pass_team = 'B';
    start_countdow();
  } else if (!strcmp(valid_passwords[2], cr_pass)) {
    state = CD_STATE;
    pass_team = 'C';
    start_countdow();
  } else {
    invalid_pass();
  }
  reset_cr_pass_input();
}

// Proccess input functions
void process_key_input() {
  input_key = keypad.getKey();

  if (input_key) {
    blink_light(activity_led_output);
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

// journey orchestration functions

void start_menu() {
  print_line("Insira a senha:", 0);
  process_key_input();
  print_line(cr_pass, 1);
}

void loop() {
  if (state == INIT_STATE) {
    start_menu();
  } else if (state == CD_STATE){

  } else {
    print_line(state, 0);
    print_digit(pass_team,1, 0);
  }
}
