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

## Wiring Diagram

### Power System

```
11.1V LiPo Battery
    |
    | (Positive)
    |
    v
[5V Step-down Converter]
    |
    | (5V Output)
    |
    v
Arduino 5V Pin
    |
    | (Ground)
    |
    v
Arduino GND Pin
    |
    | (Ground)
    |
    v
Battery Negative
```

### Arduino Pin Connections

#### Keypad (4x4 Matrix)
```
Arduino Pin  |  Keypad Connection
-------------|------------------
Pin 6        |  Row 1
Pin 7        |  Row 2  
Pin 8        |  Row 3
Pin 9        |  Row 4
Pin 2        |  Column 1
Pin 3        |  Column 2
Pin 4        |  Column 3
Pin 5        |  Column 4
```

#### LCD Display (I2C)
```
Arduino Pin  |  LCD Connection
-------------|------------------
A4 (SDA)     |  SDA
A5 (SCL)     |  SCL
5V           |  VCC
GND          |  GND
```

#### Buzzer and LED
```
Arduino Pin 13
    |
    | (Positive)
    |
    v
[Buzzer Positive] + [Resistor] + [Red LED Positive]
    |
    | (Negative)
    |
    v
Arduino GND
```

#### Siren Relay System
```
Arduino Pin 12
    |
    | (Control Signal)
    |
    v
[Relay Module] IN
    |
    | (Ground)
    |
    v
Arduino GND

Relay Module:
- VCC: 5V (from Arduino)
- GND: Arduino GND
- IN: Arduino Pin 12
- NO: (Not Used)
- NC: Siren Positive
- COM: 11.1V Battery Positive

Siren:
- Positive: Relay NC
- Negative: Battery Negative
```

### Visual Layout

#### Arduino Pinout (Top View)
```
                    ┌─────────────────┐
                    │ 5V  GND  A4 A5  │ ← I2C LCD (A4=SDA, A5=SCL)
                    │ 13  12   2  3   │ ← Pin 13: Buzzer+LED, Pin 12: Relay
                    │ 4   5    6  7   │ ← Keypad Columns (2,3,4,5)
                    │ 8   9   10 11   │ ← Keypad Rows (6,7,8,9)
                    └─────────────────┘
```

#### Keypad Layout
```
Keypad Layout:     Arduino Pins:
┌─┬─┬─┬─┐          Rows:    6  7  8  9
│1│2│3│A│          Cols:    2  3  4  5
├─┼─┼─┼─┤
│4│5│6│B│
├─┼─┼─┼─┤
│7│8│9│C│
├─┼─┼─┼─┤
│*│0│#│D│
└─┴─┴─┴─┘
```

#### Power Flow
```
[11.1V LiPo] ──┐
               │
               ├─→ [5V Step-down] ──→ [Arduino 5V]
               │
               └─→ [Relay COM] ──→ [Siren +] ──→ [Siren -] ──→ [Battery -]
```

### Component Specifications

| Component | Type | Voltage | Current | Notes |
|-----------|------|---------|---------|-------|
| Arduino Uno | Microcontroller | 5V | ~50mA | 6 digital pins used |
| 4x4 Keypad | Matrix | 5V | <1mA/key | 8 wires to Arduino |
| LCD Display | I2C 16x2 | 5V | ~20mA | Address 0x27 |
| Relay Module | 5V SPDT | 5V control | 10A switching | For siren control |
| Buzzer | Piezo | 5V | ~30mA | Connected to Pin 13 |
| LED | Red | 2V forward | 20mA | With 220Ω resistor |
| Step-down | Converter | 11.1V→5V | 2A | >85% efficiency |

### Safety Checklist

- [ ] **Power Polarity**: 11.1V battery positive/negative correct
- [ ] **Step-down Output**: Verify 5V output before connecting Arduino
- [ ] **Ground Common**: All components share common ground
- [ ] **Relay Rating**: Ensure relay can handle siren current
- [ ] **Wire Gauge**: Use appropriate gauge for 11.1V circuit
- [ ] **Insulation**: All connections properly insulated
- [ ] **Fuse**: Consider adding fuse in 11.1V circuit
- [ ] **Testing**: Test each component individually before full assembly

## Version History

- **v1.0**: Initial release with basic functionality
- **v1.1**: Added unified display system
- **v1.2**: Fixed team password override issues
- **v1.3**: Fixed LCD blank screen after error
