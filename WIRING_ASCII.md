# Airsoft Bomb System - ASCII Wiring Diagram

## Top View Layout

```
                    [11.1V LiPo Battery]
                           |
                           | (+)
                           |
                    [5V Step-down Converter]
                           |
                           | (5V)
                           |
                    [Arduino Uno]
                    ┌─────────────────┐
                    │ 5V  GND  A4 A5  │
                    │ 13  12   2  3   │
                    │ 4   5    6  7   │
                    │ 8   9   10 11   │
                    └─────────────────┘
                           │
                           | (GND)
                           |
                    [Common Ground Bus]
```

## Detailed Pin Connections

### Arduino Pinout (Top View)
```
                    ┌─────────────────┐
                    │ 5V  GND  A4 A5  │ ← I2C LCD (A4=SDA, A5=SCL)
                    │ 13  12   2  3   │ ← Pin 13: Buzzer+LED, Pin 12: Relay
                    │ 4   5    6  7   │ ← Keypad Columns (2,3,4,5)
                    │ 8   9   10 11   │ ← Keypad Rows (6,7,8,9)
                    └─────────────────┘
```

### Keypad Wiring (4x4 Matrix)
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

### Power Flow Diagram
```
[11.1V LiPo] ──┐
               │
               ├─→ [5V Step-down] ──→ [Arduino 5V]
               │
               └─→ [Relay COM] ──→ [Siren +] ──→ [Siren -] ──→ [Battery -]
```

### Signal Flow Diagram
```
[Keypad] ──→ [Arduino Pins 2-9] ──→ [Keypad Library]
[LCD I2C] ──→ [Arduino A4/A5] ──→ [LiquidCrystal_I2C]
[Buzzer] ──→ [Arduino Pin 13] ──→ [Digital Output]
[LED] ──→ [Arduino Pin 13] ──→ [Digital Output]
[Relay] ──→ [Arduino Pin 12] ──→ [Digital Output]
```

## Component Placement

### Breadboard Layout (Suggested)
```
    [5V Step-down]    [Arduino Uno]
    ┌─────────────┐   ┌─────────────┐
    │ IN    OUT   │   │ 5V  GND  A4 │
    │ 11.1V  5V   │   │ 13  12   2  │
    └─────────────┘   └─────────────┘
           │                 │
           └─────────────────┘ (5V & GND)

    [LCD I2C]        [Keypad 4x4]
    ┌─────────────┐   ┌─────────────┐
    │ VCC GND SDA │   │ R1 R2 R3 R4 │
    │ SCL         │   │ C1 C2 C3 C4 │
    └─────────────┘   └─────────────┘
           │                 │
           └─────────────────┘ (I2C & Matrix)

    [Relay Module]   [Buzzer + LED]
    ┌─────────────┐   ┌─────────────┐
    │ VCC GND IN  │   │ +    -      │
    │ NO NC COM   │   │ 220Ω Resistor│
    └─────────────┘   └─────────────┘
           │                 │
           └─────────────────┘ (Control & Audio)
```

## Safety Checklist

- [ ] **Power Polarity**: 11.1V battery positive/negative correct
- [ ] **Step-down Output**: Verify 5V output before connecting Arduino
- [ ] **Ground Common**: All components share common ground
- [ ] **Relay Rating**: Ensure relay can handle siren current
- [ ] **Wire Gauge**: Use appropriate gauge for 11.1V circuit
- [ ] **Insulation**: All connections properly insulated
- [ ] **Fuse**: Consider adding fuse in 11.1V circuit
- [ ] **Testing**: Test each component individually before full assembly

## Troubleshooting Guide

### No Power
1. Check battery voltage (should be 11.1V+)
2. Verify step-down converter output (should be 5V)
3. Check all ground connections

### LCD Not Working
1. Verify I2C address (0x27)
2. Check SDA/SCL connections
3. Ensure 5V power to LCD

### Keypad Not Responding
1. Check row/column pin connections
2. Verify continuity with multimeter
3. Test individual keys

### Siren Not Working
1. Check relay operation (should click when activated)
2. Verify 11.1V power to relay COM
3. Check siren connections to NC and ground

### Buzzer/LED Not Working
1. Check Pin 13 connection
2. Verify resistor value (220Ω)
3. Check polarity of LED
