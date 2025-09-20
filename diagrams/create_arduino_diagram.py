#!/usr/bin/env python3
"""
Create a proper Arduino Uno diagram using schemdraw
Based on the official schemdraw documentation
"""

import schemdraw
import schemdraw.elements as elm

# Custom Arduino class based on the schemdraw documentation
class Atmega328(elm.Ic):
    def __init__(self, *args, **kwargs):
        pins=[elm.IcPin(name='PD0', pin='2', side='r', slot='1/22'),
              elm.IcPin(name='PD1', pin='3', side='r', slot='2/22'),
              elm.IcPin(name='PD2', pin='4', side='r', slot='3/22'),
              elm.IcPin(name='PD3', pin='5', side='r', slot='4/22'),
              elm.IcPin(name='PD4', pin='6', side='r', slot='5/22'),
              elm.IcPin(name='PD5', pin='11', side='r', slot='6/22'),
              elm.IcPin(name='PD6', pin='12', side='r', slot='7/22'),
              elm.IcPin(name='PD7', pin='13', side='r', slot='8/22'),
              elm.IcPin(name='PC0', pin='23', side='r', slot='10/22'),
              elm.IcPin(name='PC1', pin='24', side='r', slot='11/22'),
              elm.IcPin(name='PC2', pin='25', side='r', slot='12/22'),
              elm.IcPin(name='PC3', pin='26', side='r', slot='13/22'),
              elm.IcPin(name='PC4', pin='27', side='r', slot='14/22'),
              elm.IcPin(name='PC5', pin='28', side='r', slot='15/22'),
              elm.IcPin(name='PB0', pin='14', side='r', slot='17/22'),
              elm.IcPin(name='PB1', pin='15', side='r', slot='18/22'),
              elm.IcPin(name='PB2', pin='16', side='r', slot='19/22'),
              elm.IcPin(name='PB3', pin='17', side='r', slot='20/22'),
              elm.IcPin(name='PB4', pin='18', side='r', slot='21/22'),
              elm.IcPin(name='PB5', pin='19', side='r', slot='22/22'),

              elm.IcPin(name='RESET', side='l', slot='22/22', invert=True, pin='1'),
              elm.IcPin(name='XTAL2', side='l', slot='19/22', pin='10'),
              elm.IcPin(name='XTAL1', side='l', slot='17/22', pin='9'),
              elm.IcPin(name='AREF', side='l', slot='15/22', pin='21'),
              elm.IcPin(name='AVCC', side='l', slot='14/22', pin='20'),
              elm.IcPin(name='AGND', side='l', slot='13/22', pin='22'),
              elm.IcPin(name='VCC', side='l', slot='11/22', pin='7'),
              elm.IcPin(name='GND', side='l', slot='10/22', pin='8')]
        super().__init__(pins=pins, w=5, plblofst=.05, botlabel='ATMEGA328', **kwargs)

def create_arduino_diagram():
    """Create a proper Arduino Uno diagram with all connections"""
    with schemdraw.Drawing(file='arduino_uno.svg') as d:
        d.config(fontsize=11, inches_per_unit=.4)
        
        # Arduino Uno board
        Q1 = Atmega328()
        
        # Digital pins header (JP4)
        JP4 = (elm.Header(rows=10, shownumber=True,
                          pinsright=['D8', 'D9', 'D10', 'D11', 'D12', 'D13', '', '', '', ''],
                          pinalignright='center')
                    .flip().at(Q1.PB5, dx=4, dy=1).anchor('pin6').label('JP4', fontsize=10))

        # Analog pins header (JP3)
        JP3 = (elm.Header(rows=6, shownumber=True, 
                          pinsright=['A0', 'A1', 'A2', 'A3', 'A4', 'A5'], 
                          pinalignright='center')
                        .flip().at(Q1.PC5, dx=4).anchor('pin6').label('JP3', fontsize=10))

        # Digital pins header (JP2)
        JP2 = (elm.Header(rows=8, shownumber=True, 
                          pinsright=['D0', 'D1', 'D2', 'D3', 'D4', 'D5', 'D6', 'D7'],
                          pinalignright='center')).at(Q1.PD7, dx=3).flip().anchor('pin8').label('JP2', fontsize=10)

        # Connect digital pins
        elm.OrthoLines(n=6).at(Q1.PB5).to(JP4.pin6)
        elm.OrthoLines(n=6).at(Q1.PC5).to(JP3.pin6)
        elm.OrthoLines(n=8).at(Q1.PD7).to(JP2.pin8)

        # Power and reference pins
        elm.Line().left(.9).at(JP4.pin7).label('GND', 'left')
        elm.Line().left(.9).at(JP4.pin8).label('AREF', 'left')
        elm.Line().left(.9).at(JP4.pin9).label('AD4/SDA', 'left')
        elm.Line().left(.9).at(JP4.pin10).label('AD5/SCL', 'left')

        # USB/Serial header (JP1)
        JP1 = (elm.Header(rows=6, shownumber=True, 
                          pinsright=['VCC', 'RXD', 'TXD', 'DTR', 'RTS', 'GND'],
                          pinalignright='center').right().at(Q1.PD0, dx=4, dy=-2).anchor('pin1'))
        
        # Power connections
        elm.Line().left(d.unit/2).at(JP1.pin1)
        elm.Vdd().label('+5V')
        elm.Line().left().at(JP1.pin2)
        elm.Line().toy(Q1.PD0).dot()
        elm.Line().left(d.unit+.6).at(JP1.pin3)
        elm.Line().toy(Q1.PD1).dot()
        elm.Line().left(d.unit/2).at(JP1.pin6)
        elm.Ground()

        # Crystal oscillator
        elm.Line().left(d.unit*2).at(Q1.XTAL2).dot()
        d.push()
        elm.Capacitor().left(d.unit/2).scale(.75)
        elm.Line().toy(Q1.XTAL1).dot()
        elm.Ground()
        elm.Capacitor().right(d.unit/2).scale(.75).dot()
        d.pop()
        elm.Crystal().toy(Q1.XTAL1).label('16MHz', 'bottom')
        elm.Line().tox(Q1.XTAL1)

        # Reference voltage
        elm.Line().left(d.unit/3).at(Q1.AREF).label('AREF', 'left')
        elm.Line().left(1.5*d.unit).at(Q1.AVCC)
        elm.Vdd().label('+5V')
        elm.Line().toy(Q1.VCC).dot().idot()
        elm.Line().tox(Q1.VCC).hold()
        elm.Capacitor().down().label('100n')
        GND = elm.Ground()

        # Ground connections
        elm.Line().left().at(Q1.AGND)
        elm.Line().toy(Q1.GND).dot()
        elm.Line().tox(Q1.GND).hold()
        elm.Wire('|-').to(GND.center).dot()

        # Reset circuit
        elm.Line().left().at(Q1.RESET).dot()
        d.push()
        elm.RBox().up().label('10K')
        elm.Vdd().label('+5V')
        d.pop()
        elm.Line().left().dot()
        d.push()
        RST = elm.Button().up().label('Reset')
        elm.Line().left(d.unit/2)
        elm.Ground()
        d.pop()

        # DTR capacitor
        elm.Capacitor().left().at(JP1.pin4).label('100n', 'bottom')
        elm.Wire('c', k=-16).to(RST.start)

if __name__ == "__main__":
    print("Creating Arduino Uno diagram...")
    create_arduino_diagram()
    print("Arduino Uno diagram created: arduino_uno.svg")
