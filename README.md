# Airsoft Bomb System

A realistic airsoft bomb simulation system built with Arduino that features countdown timers, team-based password authentication, and various alarm states.

## Features

- **Team-based Authentication**: 3 team passwords (A, B, C) + master password
- **Countdown Timer**: Configurable countdown with visual display
- **Multiple Alarm States**: Explosion, disarm, premature explosion, self-destruct
- **Admin Mode**: Master password access for configuration
- **Visual Feedback**: 16x2 LCD display with activity LED
- **Audio Alerts**: Buzzer and siren with different patterns
- **Security**: Prevents team password override in final states

## Hardware Requirements

- Arduino Uno/Nano
- 16x2 I2C LCD Display (0x27 address)
- 4x4 Matrix Keypad
- Buzzer (5V)
- Red LED + Resistor
- Relay Module (5V)
- Siren (11.1V compatible)
- 11.1V LiPo Battery
- 5V Step-down Converter
- Jumper wires and breadboard

## System States

### 1. INIT_STATE
- **Display**: "Insira a senha:" (Enter password)
- **Actions**: 
  - Team password → Start countdown
  - Master password → Admin mode
  - Invalid password → Error sequence

### 2. CD_STATE (Countdown)
- **Display**: "Ativacao: MM:SS" (Activation timer)
- **Actions**:
  - Team password → Disarm bomb
  - Timer reaches 0 → Explosion
  - Invalid password → Error sequence

### 3. EXPLOSION_STATE
- **Display**: "BOOOM! Senha: [Team]"
- **Actions**: Only master password can reset

### 4. DISARMED_STATE
- **Display**: "Desarmada! [Team]"
- **Actions**: Only master password can reset

### 5. PREMATURE_EXPLOSION_STATE
- **Display**: "Ooops! Senha: [Team]"
- **Actions**: Only master password can reset

### 6. ADM_STATE (Admin Mode)
- **Display**: Admin menu options
- **Actions**:
  - Timer configuration (1)
  - Password management (2)
  - Exit (#)

## Default Passwords

- **Master Password**: `*5456*`
- **Team A**: `123456`
- **Team B**: `987654`
- **Team C**: `741852`

## Configuration

### Timer Settings
- Default: 10 minutes
- Range: 00-99 minutes
- Set via admin mode or direct input

### Password Management
- Change team passwords via admin mode
- Master password cannot be changed via interface
- Passwords are 6 digits (0-9, *)

## Safety Features

- **Watchdog Timer**: 8-second timeout prevents system hangs
- **Team Lock**: Final states only accept master password
- **Error Handling**: Visual and audio feedback for invalid inputs
- **Debouncing**: Prevents accidental key presses

## Installation

1. Upload the Arduino sketch to your board
2. Connect hardware according to wiring diagram
3. Power on with 11.1V LiPo battery
4. System initializes and shows "Test LCD"
5. Ready for use after initialization

## Usage

### Basic Operation
1. Enter team password to arm bomb
2. Countdown begins automatically
3. Enter same team password to disarm
4. Use master password to reset system

### Admin Functions
1. Enter master password from init state
2. Select option (1=Timer, 2=Password)
3. Configure as needed
4. Press # to exit admin mode

## Troubleshooting

### LCD Issues
- Check I2C address (default 0x27)
- Verify 5V power supply
- Check SDA/SCL connections

### Keypad Issues
- Verify row/column pin connections
- Check for loose connections
- Test individual keys

### Power Issues
- Ensure 5V step-down converter is working
- Check battery voltage (11.1V nominal)
- Verify ground connections

## Technical Details

### Pin Configuration
- **Keypad Rows**: Pins 6, 7, 8, 9
- **Keypad Columns**: Pins 2, 3, 4, 5
- **LCD I2C**: SDA (A4), SCL (A5)
- **Buzzer/LED**: Pin 13
- **Siren Relay**: Pin 12

### Power Requirements
- **Input**: 11.1V LiPo battery
- **Arduino**: 5V (via step-down converter)
- **Siren**: 11.1V (via relay)

### Memory Usage
- **Flash**: ~8KB
- **SRAM**: ~1.5KB
- **EEPROM**: Not used

## License

This project is for educational and recreational use only. Use responsibly and follow all local laws and regulations.

## Version History

- **v1.0**: Initial release with basic functionality
- **v1.1**: Added unified display system
- **v1.2**: Fixed team password override issues
- **v1.3**: Fixed LCD blank screen after error
