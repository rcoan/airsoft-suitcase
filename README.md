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

## Circuit Diagrams

### Main Power System

::_schemdraw_:: alt="Power System Circuit" color="white"
    # Power source
    (BAT := elm.SourceV().label('11.1V LiPo\nBattery').up())
    elm.Line().right(drawing.unit*1.5)
    
    # Step-down converter
    (CONV := elm.Box().label('5V Step-down\nConverter').right())
    elm.Line().right(drawing.unit*0.5)
    
    # Arduino power input
    (ARDUINO := elm.Box().label('Arduino Uno\n5V Input').right())
    
    # Ground connections
    elm.Line().down(drawing.unit*1.5).at(BAT.start)
    (GND := elm.Ground())
    elm.Line().right(drawing.unit*4).at(GND.start)
    elm.Line().up(drawing.unit*0.5).at(ARDUINO.start)
    
    # Labels
    elm.Line().label('+11.1V', loc='top').at(BAT.end)
    elm.Line().label('+5V', loc='top').at(ARDUINO.end)
    elm.Line().label('GND', loc='bot').at(GND.start)
::end-schemdraw::

### Arduino Pin Connections

::_schemdraw_:: alt="Arduino Pinout Diagram" color="white"
    # Arduino board representation
    (ARDUINO := elm.Box().label('Arduino Uno').up())
    
    # Power pins
    elm.Line().right(drawing.unit*0.5).at(ARDUINO.end)
    (VCC := elm.Dot().label('5V'))
    elm.Line().right(drawing.unit*0.5)
    (GND_PIN := elm.Dot().label('GND'))
    
    # I2C pins for LCD
    elm.Line().down(drawing.unit*0.3).at(ARDUINO.end)
    elm.Line().right(drawing.unit*1.5)
    (SDA := elm.Dot().label('A4 (SDA)'))
    elm.Line().right(drawing.unit*0.5)
    (SCL := elm.Dot().label('A5 (SCL)'))
    
    # Digital pins
    elm.Line().down(drawing.unit*0.6).at(ARDUINO.end)
    elm.Line().right(drawing.unit*0.5)
    (PIN12 := elm.Dot().label('Pin 12\n(Relay)'))
    elm.Line().right(drawing.unit*0.5)
    (PIN13 := elm.Dot().label('Pin 13\n(Buzzer/LED)'))
    
    # Keypad pins
    elm.Line().down(drawing.unit*0.9).at(ARDUINO.end)
    elm.Line().right(drawing.unit*0.5)
    (PIN2 := elm.Dot().label('Pin 2\n(Col1)'))
    elm.Line().right(drawing.unit*0.5)
    (PIN3 := elm.Dot().label('Pin 3\n(Col2)'))
    elm.Line().right(drawing.unit*0.5)
    (PIN4 := elm.Dot().label('Pin 4\n(Col3)'))
    elm.Line().right(drawing.unit*0.5)
    (PIN5 := elm.Dot().label('Pin 5\n(Col4)'))
    
    elm.Line().down(drawing.unit*1.2).at(ARDUINO.end)
    elm.Line().right(drawing.unit*0.5)
    (PIN6 := elm.Dot().label('Pin 6\n(Row1)'))
    elm.Line().right(drawing.unit*0.5)
    (PIN7 := elm.Dot().label('Pin 7\n(Row2)'))
    elm.Line().right(drawing.unit*0.5)
    (PIN8 := elm.Dot().label('Pin 8\n(Row3)'))
    elm.Line().right(drawing.unit*0.5)
    (PIN9 := elm.Dot().label('Pin 9\n(Row4)'))
::end-schemdraw::

### Keypad Matrix Circuit

::_schemdraw_:: alt="4x4 Keypad Matrix" color="white"
    # Keypad matrix representation
    (KP := elm.Box().label('4x4 Keypad Matrix').up())
    
    # Row connections
    elm.Line().left(drawing.unit*1).at(KP.end)
    (R1 := elm.Dot().label('Row 1\nPin 6'))
    elm.Line().down(drawing.unit*0.3)
    (R2 := elm.Dot().label('Row 2\nPin 7'))
    elm.Line().down(drawing.unit*0.3)
    (R3 := elm.Dot().label('Row 3\nPin 8'))
    elm.Line().down(drawing.unit*0.3)
    (R4 := elm.Dot().label('Row 4\nPin 9'))
    
    # Column connections
    elm.Line().right(drawing.unit*1).at(KP.end)
    (C1 := elm.Dot().label('Col 1\nPin 2'))
    elm.Line().down(drawing.unit*0.3)
    (C2 := elm.Dot().label('Col 2\nPin 3'))
    elm.Line().down(drawing.unit*0.3)
    (C3 := elm.Dot().label('Col 3\nPin 4'))
    elm.Line().down(drawing.unit*0.3)
    (C4 := elm.Dot().label('Col 4\nPin 5'))
    
    # Keypad layout
    elm.Line().right(drawing.unit*0.5).at(KP.end)
    elm.Line().down(drawing.unit*0.5)
    elm.Line().left(drawing.unit*0.5)
    elm.Line().up(drawing.unit*0.5)
    elm.Line().right(drawing.unit*0.5)
    elm.Line().down(drawing.unit*0.5)
    elm.Line().left(drawing.unit*0.5)
    elm.Line().up(drawing.unit*0.5)
    elm.Line().right(drawing.unit*0.5)
    elm.Line().down(drawing.unit*0.5)
    elm.Line().left(drawing.unit*0.5)
    elm.Line().up(drawing.unit*0.5)
    elm.Line().right(drawing.unit*0.5)
    elm.Line().down(drawing.unit*0.5)
    elm.Line().left(drawing.unit*0.5)
    elm.Line().up(drawing.unit*0.5)
::end-schemdraw::

### LCD I2C Connection

::_schemdraw_:: alt="LCD I2C Connection" color="white"
    # Arduino I2C pins
    (ARDUINO := elm.Box().label('Arduino').left())
    elm.Line().right(drawing.unit*1)
    (SDA_PIN := elm.Dot().label('A4 (SDA)'))
    elm.Line().right(drawing.unit*0.5)
    (SCL_PIN := elm.Dot().label('A5 (SCL)'))
    
    # I2C bus
    elm.Line().down(drawing.unit*0.5).at(SDA_PIN.start)
    elm.Line().right(drawing.unit*2)
    elm.Line().up(drawing.unit*0.5)
    
    # LCD module
    (LCD := elm.Box().label('16x2 LCD\nI2C (0x27)').right())
    
    # Power connections
    elm.Line().down(drawing.unit*1).at(ARDUINO.end)
    (VCC := elm.Dot().label('5V'))
    elm.Line().right(drawing.unit*2)
    elm.Line().up(drawing.unit*0.5)
    (GND := elm.Dot().label('GND'))
    elm.Line().right(drawing.unit*1)
    elm.Line().up(drawing.unit*0.5)
    
    # Pull-up resistors
    elm.Line().up(drawing.unit*0.3).at(SDA_PIN.start)
    (R1 := elm.Resistor().up().label('4.7kΩ'))
    elm.Line().right(drawing.unit*0.5)
    elm.Line().to(VCC.start)
    
    elm.Line().up(drawing.unit*0.3).at(SCL_PIN.start)
    (R2 := elm.Resistor().up().label('4.7kΩ'))
    elm.Line().right(drawing.unit*0.5)
    elm.Line().to(VCC.start)
::end-schemdraw::

### Buzzer and LED Circuit

::_schemdraw_:: alt="Buzzer and LED Circuit" color="white"
    # Arduino pin 13
    (ARDUINO := elm.Box().label('Arduino\nPin 13').left())
    elm.Line().right(drawing.unit*1)
    (PIN13 := elm.Dot().label('Pin 13'))
    
    # Buzzer
    elm.Line().down(drawing.unit*0.5)
    (BUZZER := elm.Speaker().right().label('Buzzer\n5V'))
    elm.Line().down(drawing.unit*0.5)
    (GND1 := elm.Ground())
    
    # LED with resistor
    elm.Line().up(drawing.unit*0.5).at(PIN13.start)
    (RESISTOR := elm.Resistor().right().label('220Ω'))
    (LED := elm.LED().right().label('Red LED'))
    elm.Line().down(drawing.unit*0.5)
    (GND2 := elm.Ground())
    
    # Common ground
    elm.Line().left(drawing.unit*1).at(GND1.start)
    elm.Line().up(drawing.unit*1)
    elm.Line().right(drawing.unit*1)
    elm.Line().down(drawing.unit*0.5)
::end-schemdraw::

### Siren Relay Circuit

::_schemdraw_:: alt="Siren Relay Circuit" color="white"
    # Arduino control
    (ARDUINO := elm.Box().label('Arduino\nPin 12').left())
    elm.Line().right(drawing.unit*1)
    (PIN12 := elm.Dot().label('Pin 12'))
    
    # Relay module
    elm.Line().down(drawing.unit*0.5)
    (RELAY := elm.Box().label('5V Relay\nModule').right())
    
    # Relay connections
    elm.Line().right(drawing.unit*1).at(RELAY.end)
    (COM := elm.Dot().label('COM'))
    elm.Line().down(drawing.unit*0.3)
    (NC := elm.Dot().label('NC'))
    elm.Line().down(drawing.unit*0.3)
    (NO := elm.Dot().label('NO'))
    
    # Siren circuit
    elm.Line().right(drawing.unit*1).at(COM.start)
    (BATTERY := elm.SourceV().label('11.1V\nLiPo').up())
    elm.Line().left(drawing.unit*0.5)
    elm.Line().down(drawing.unit*0.5)
    (SIREN := elm.Speaker().right().label('Siren\n11.1V'))
    elm.Line().down(drawing.unit*0.5)
    (GND := elm.Ground())
    
    # Relay control power
    elm.Line().up(drawing.unit*0.5).at(RELAY.start)
    (VCC := elm.Dot().label('5V'))
    elm.Line().down(drawing.unit*0.3)
    (GND_CTRL := elm.Dot().label('GND'))
    
    # Ground connections
    elm.Line().left(drawing.unit*1).at(GND_CTRL.start)
    elm.Line().down(drawing.unit*0.5)
    elm.Line().right(drawing.unit*2)
    elm.Line().up(drawing.unit*0.5)
::end-schemdraw::

### Complete System Overview

::_schemdraw_:: alt="Complete System Overview" color="white"
    # Main power system
    (BATTERY := elm.SourceV().label('11.1V LiPo').up())
    elm.Line().right(drawing.unit*1)
    (CONVERTER := elm.Box().label('5V\nConverter').right())
    elm.Line().right(drawing.unit*1)
    (ARDUINO := elm.Box().label('Arduino\nUno').right())
    
    # Ground
    elm.Line().down(drawing.unit*1.5).at(BATTERY.start)
    (GND := elm.Ground())
    elm.Line().right(drawing.unit*3).at(GND.start)
    elm.Line().up(drawing.unit*0.5)
    
    # Keypad
    elm.Line().up(drawing.unit*0.5).at(ARDUINO.end)
    (KEYPAD := elm.Box().label('4x4\nKeypad').right())
    
    # LCD
    elm.Line().down(drawing.unit*0.5).at(ARDUINO.end)
    (LCD := elm.Box().label('16x2 LCD\nI2C').right())
    
    # Buzzer/LED
    elm.Line().down(drawing.unit*0.5).at(ARDUINO.end)
    (BUZZER := elm.Speaker().right().label('Buzzer\n+ LED'))
    
    # Relay and Siren
    elm.Line().down(drawing.unit*0.5).at(ARDUINO.end)
    (RELAY := elm.Box().label('5V\nRelay').right())
    elm.Line().right(drawing.unit*1)
    (SIREN := elm.Speaker().right().label('Siren\n11.1V'))
    
    # Siren power from battery
    elm.Line().left(drawing.unit*2).at(SIREN.start)
    elm.Line().up(drawing.unit*1)
    elm.Line().left(drawing.unit*1)
    elm.Line().to(BATTERY.end)
::end-schemdraw::

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
