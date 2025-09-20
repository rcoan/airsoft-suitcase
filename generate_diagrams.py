#!/usr/bin/env python3
"""
Generate circuit diagrams for the airsoft bomb system using schemdraw
"""

import schemdraw
import schemdraw.elements as elm
import os

def create_power_system():
    """Generate main power system diagram"""
    d = schemdraw.Drawing()
    
    # Power source
    d += (BAT := elm.SourceV().label('11.1V LiPo\nBattery').up())
    d += elm.Line().right(d.unit*1.5)
    
    # Step-down converter
    d += (CONV := elm.RBox().label('5V Step-down\nConverter').right())
    d += elm.Line().right(d.unit*0.5)
    
    # Arduino power input
    d += (ARDUINO := elm.RBox().label('Arduino Uno\n5V Input').right())
    
    # Ground connections
    d += elm.Line().down(d.unit*1.5).at(BAT.start)
    d += (GND := elm.Ground())
    d += elm.Line().right(d.unit*4).at(GND.start)
    d += elm.Line().up(d.unit*0.5).at(ARDUINO.start)
    
    # Labels
    d += elm.Line().label('+11.1V', loc='top').at(BAT.end)
    d += elm.Line().label('+5V', loc='top').at(ARDUINO.end)
    d += elm.Line().label('GND', loc='bot').at(GND.start)
    
    return d

def create_arduino_pinout():
    """Generate Arduino pinout diagram"""
    d = schemdraw.Drawing()
    
    # Arduino board representation
    d += (ARDUINO := elm.RBox().label('Arduino Uno').up())
    
    # Power pins
    d += elm.Line().right(d.unit*0.5).at(ARDUINO.end)
    d += (VCC := elm.Dot().label('5V'))
    d += elm.Line().right(d.unit*0.5)
    d += (GND_PIN := elm.Dot().label('GND'))
    
    # I2C pins for LCD
    d += elm.Line().down(d.unit*0.3).at(ARDUINO.end)
    d += elm.Line().right(d.unit*1.5)
    d += (SDA := elm.Dot().label('A4 (SDA)'))
    d += elm.Line().right(d.unit*0.5)
    d += (SCL := elm.Dot().label('A5 (SCL)'))
    
    # Digital pins
    d += elm.Line().down(d.unit*0.6).at(ARDUINO.end)
    d += elm.Line().right(d.unit*0.5)
    d += (PIN12 := elm.Dot().label('Pin 12\n(Relay)'))
    d += elm.Line().right(d.unit*0.5)
    d += (PIN13 := elm.Dot().label('Pin 13\n(Buzzer/LED)'))
    
    # Keypad pins
    d += elm.Line().down(d.unit*0.9).at(ARDUINO.end)
    d += elm.Line().right(d.unit*0.5)
    d += (PIN2 := elm.Dot().label('Pin 2\n(Col1)'))
    d += elm.Line().right(d.unit*0.5)
    d += (PIN3 := elm.Dot().label('Pin 3\n(Col2)'))
    d += elm.Line().right(d.unit*0.5)
    d += (PIN4 := elm.Dot().label('Pin 4\n(Col3)'))
    d += elm.Line().right(d.unit*0.5)
    d += (PIN5 := elm.Dot().label('Pin 5\n(Col4)'))
    
    d += elm.Line().down(d.unit*1.2).at(ARDUINO.end)
    d += elm.Line().right(d.unit*0.5)
    d += (PIN6 := elm.Dot().label('Pin 6\n(Row1)'))
    d += elm.Line().right(d.unit*0.5)
    d += (PIN7 := elm.Dot().label('Pin 7\n(Row2)'))
    d += elm.Line().right(d.unit*0.5)
    d += (PIN8 := elm.Dot().label('Pin 8\n(Row3)'))
    d += elm.Line().right(d.unit*0.5)
    d += (PIN9 := elm.Dot().label('Pin 9\n(Row4)'))
    
    return d

def create_keypad_matrix():
    """Generate keypad matrix diagram"""
    d = schemdraw.Drawing()
    
    # Keypad matrix representation
    d += (KP := elm.RBox().label('4x4 Keypad Matrix').up())
    
    # Row connections
    d += elm.Line().left(d.unit*1).at(KP.end)
    d += (R1 := elm.Dot().label('Row 1\nPin 6'))
    d += elm.Line().down(d.unit*0.3)
    d += (R2 := elm.Dot().label('Row 2\nPin 7'))
    d += elm.Line().down(d.unit*0.3)
    d += (R3 := elm.Dot().label('Row 3\nPin 8'))
    d += elm.Line().down(d.unit*0.3)
    d += (R4 := elm.Dot().label('Row 4\nPin 9'))
    
    # Column connections
    d += elm.Line().right(d.unit*1).at(KP.end)
    d += (C1 := elm.Dot().label('Col 1\nPin 2'))
    d += elm.Line().down(d.unit*0.3)
    d += (C2 := elm.Dot().label('Col 2\nPin 3'))
    d += elm.Line().down(d.unit*0.3)
    d += (C3 := elm.Dot().label('Col 3\nPin 4'))
    d += elm.Line().down(d.unit*0.3)
    d += (C4 := elm.Dot().label('Col 4\nPin 5'))
    
    return d

def create_lcd_i2c():
    """Generate LCD I2C connection diagram"""
    d = schemdraw.Drawing()
    
    # Arduino I2C pins
    d += (ARDUINO := elm.RBox().label('Arduino').left())
    d += elm.Line().right(d.unit*1)
    d += (SDA_PIN := elm.Dot().label('A4 (SDA)'))
    d += elm.Line().right(d.unit*0.5)
    d += (SCL_PIN := elm.Dot().label('A5 (SCL)'))
    
    # I2C bus
    d += elm.Line().down(d.unit*0.5).at(SDA_PIN.start)
    d += elm.Line().right(d.unit*2)
    d += elm.Line().up(d.unit*0.5)
    
    # LCD module
    d += (LCD := elm.RBox().label('16x2 LCD\nI2C (0x27)').right())
    
    # Power connections
    d += elm.Line().down(d.unit*1).at(ARDUINO.end)
    d += (VCC := elm.Dot().label('5V'))
    d += elm.Line().right(d.unit*2)
    d += elm.Line().up(d.unit*0.5)
    d += (GND := elm.Dot().label('GND'))
    d += elm.Line().right(d.unit*1)
    d += elm.Line().up(d.unit*0.5)
    
    # Pull-up resistors
    d += elm.Line().up(d.unit*0.3).at(SDA_PIN.start)
    d += (R1 := elm.Resistor().up().label('4.7kΩ'))
    d += elm.Line().right(d.unit*0.5)
    d += elm.Line().to(VCC.start)
    
    d += elm.Line().up(d.unit*0.3).at(SCL_PIN.start)
    d += (R2 := elm.Resistor().up().label('4.7kΩ'))
    d += elm.Line().right(d.unit*0.5)
    d += elm.Line().to(VCC.start)
    
    return d

def create_buzzer_led():
    """Generate buzzer and LED circuit diagram"""
    d = schemdraw.Drawing()
    
    # Arduino pin 13
    d += (ARDUINO := elm.RBox().label('Arduino\nPin 13').left())
    d += elm.Line().right(d.unit*1)
    d += (PIN13 := elm.Dot().label('Pin 13'))
    
    # Buzzer
    d += elm.Line().down(d.unit*0.5)
    d += (BUZZER := elm.Speaker().right().label('Buzzer\n5V'))
    d += elm.Line().down(d.unit*0.5)
    d += (GND1 := elm.Ground())
    
    # LED with resistor
    d += elm.Line().up(d.unit*0.5).at(PIN13.start)
    d += (RESISTOR := elm.Resistor().right().label('220Ω'))
    d += (LED := elm.LED().right().label('Red LED'))
    d += elm.Line().down(d.unit*0.5)
    d += (GND2 := elm.Ground())
    
    # Common ground
    d += elm.Line().left(d.unit*1).at(GND1.start)
    d += elm.Line().up(d.unit*1)
    d += elm.Line().right(d.unit*1)
    d += elm.Line().down(d.unit*0.5)
    
    return d

def create_siren_relay():
    """Generate siren relay circuit diagram"""
    d = schemdraw.Drawing()
    
    # Arduino control
    d += (ARDUINO := elm.RBox().label('Arduino\nPin 12').left())
    d += elm.Line().right(d.unit*1)
    d += (PIN12 := elm.Dot().label('Pin 12'))
    
    # Relay module
    d += elm.Line().down(d.unit*0.5)
    d += (RELAY := elm.RBox().label('5V Relay\nModule').right())
    
    # Relay connections
    d += elm.Line().right(d.unit*1).at(RELAY.end)
    d += (COM := elm.Dot().label('COM'))
    d += elm.Line().down(d.unit*0.3)
    d += (NC := elm.Dot().label('NC'))
    d += elm.Line().down(d.unit*0.3)
    d += (NO := elm.Dot().label('NO'))
    
    # Siren circuit
    d += elm.Line().right(d.unit*1).at(COM.start)
    d += (BATTERY := elm.SourceV().label('11.1V\nLiPo').up())
    d += elm.Line().left(d.unit*0.5)
    d += elm.Line().down(d.unit*0.5)
    d += (SIREN := elm.Speaker().right().label('Siren\n11.1V'))
    d += elm.Line().down(d.unit*0.5)
    d += (GND := elm.Ground())
    
    # Relay control power
    d += elm.Line().up(d.unit*0.5).at(RELAY.start)
    d += (VCC := elm.Dot().label('5V'))
    d += elm.Line().down(d.unit*0.3)
    d += (GND_CTRL := elm.Dot().label('GND'))
    
    # Ground connections
    d += elm.Line().left(d.unit*1).at(GND_CTRL.start)
    d += elm.Line().down(d.unit*0.5)
    d += elm.Line().right(d.unit*2)
    d += elm.Line().up(d.unit*0.5)
    
    return d

def create_system_overview():
    """Generate complete system overview diagram"""
    d = schemdraw.Drawing()
    
    # Main power system
    d += (BATTERY := elm.SourceV().label('11.1V LiPo').up())
    d += elm.Line().right(d.unit*1)
    d += (CONVERTER := elm.RBox().label('5V\nConverter').right())
    d += elm.Line().right(d.unit*1)
    d += (ARDUINO := elm.RBox().label('Arduino\nUno').right())
    
    # Ground
    d += elm.Line().down(d.unit*1.5).at(BATTERY.start)
    d += (GND := elm.Ground())
    d += elm.Line().right(d.unit*3).at(GND.start)
    d += elm.Line().up(d.unit*0.5)
    
    # Keypad
    d += elm.Line().up(d.unit*0.5).at(ARDUINO.end)
    d += (KEYPAD := elm.RBox().label('4x4\nKeypad').right())
    
    # LCD
    d += elm.Line().down(d.unit*0.5).at(ARDUINO.end)
    d += (LCD := elm.RBox().label('16x2 LCD\nI2C').right())
    
    # Buzzer/LED
    d += elm.Line().down(d.unit*0.5).at(ARDUINO.end)
    d += (BUZZER := elm.Speaker().right().label('Buzzer\n+ LED'))
    
    # Relay and Siren
    d += elm.Line().down(d.unit*0.5).at(ARDUINO.end)
    d += (RELAY := elm.RBox().label('5V\nRelay').right())
    d += elm.Line().right(d.unit*1)
    d += (SIREN := elm.Speaker().right().label('Siren\n11.1V'))
    
    # Siren power from battery
    d += elm.Line().left(d.unit*2)
    d += elm.Line().up(d.unit*1)
    d += elm.Line().left(d.unit*1)
    d += elm.Line().to(BATTERY.end)
    
    return d

def main():
    """Generate all circuit diagrams"""
    # Create diagrams directory
    os.makedirs('diagrams', exist_ok=True)
    
    # Generate all diagrams
    diagrams = {
        'power_system': create_power_system(),
        'arduino_pinout': create_arduino_pinout(),
        'keypad_matrix': create_keypad_matrix(),
        'lcd_i2c': create_lcd_i2c(),
        'buzzer_led': create_buzzer_led(),
        'siren_relay': create_siren_relay(),
        'system_overview': create_system_overview()
    }
    
    # Save each diagram as SVG and PNG
    for name, diagram in diagrams.items():
        print(f"Generating {name}...")
        
        # Save as SVG
        diagram.save(f'diagrams/{name}.svg')
        
        # Save as PNG
        diagram.save(f'diagrams/{name}.png', dpi=300)
    
    print("All diagrams generated successfully!")

if __name__ == "__main__":
    main()
