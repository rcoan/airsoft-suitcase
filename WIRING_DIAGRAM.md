# Airsoft Bomb System - Wiring Diagram

## Power System

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

## Arduino Pin Connections

### Keypad (4x4 Matrix)
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

### LCD Display (I2C)
```
Arduino Pin  |  LCD Connection
-------------|------------------
A4 (SDA)     |  SDA
A5 (SCL)     |  SCL
5V           |  VCC
GND          |  GND
```

### Buzzer and LED
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

### Siren Relay System
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

## Complete Wiring Summary

### Power Distribution
1. **11.1V LiPo Battery** → **5V Step-down Converter** → **Arduino 5V**
2. **Battery Ground** → **Arduino GND** (common ground)
3. **11.1V Battery** → **Relay COM** (for siren power)

### Signal Connections
1. **Keypad**: 8 wires to Arduino pins 2-9
2. **LCD**: 4 wires (VCC, GND, SDA, SCL)
3. **Buzzer/LED**: 2 wires (Pin 13, GND)
4. **Relay**: 3 wires (VCC, GND, IN)

### Safety Notes
- **Double-check polarity** on all connections
- **Use appropriate wire gauge** for 11.1V siren circuit
- **Test relay operation** before connecting siren
- **Verify step-down converter** output is 5V
- **Insulate all connections** to prevent shorts

## Component Specifications

### Arduino Uno
- Operating Voltage: 5V
- Input Voltage: 7-12V (via step-down converter)
- Digital Pins: 14 (6 used)
- Analog Pins: 6 (2 used for I2C)

### 4x4 Keypad
- Matrix type: 4x4
- Voltage: 5V compatible
- Current: <1mA per key

### LCD Display (I2C)
- Type: 16x2 Character LCD
- Interface: I2C (0x27 address)
- Voltage: 5V
- Current: ~20mA

### Relay Module
- Type: 5V SPDT Relay
- Switching Voltage: 11.1V DC
- Switching Current: 10A (siren dependent)
- Control Voltage: 5V

### Buzzer
- Type: Piezo Buzzer
- Voltage: 5V
- Current: ~30mA

### LED
- Type: Red LED
- Voltage: 2V forward
- Current: 20mA (with resistor)

### Step-down Converter
- Input: 11.1V (LiPo)
- Output: 5V
- Current: 2A minimum
- Efficiency: >85%
