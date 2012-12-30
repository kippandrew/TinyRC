tiny-rc-serial
==============

About
-----
The tiny RC serial interface is a open source hardware device for
converting servo signals from RC receivers (such as the Spektrum
AR6210) into a TTL serial stream that can be read by a microcontroller
such as an Arduino. 

The tiny RC serial interface works by timing the pulses received on up to
6 channels, and using an ATTINY84 microcontroller to convert those
pulses into a serial packet stream.

Hardware
--------

The tiny RC serial interface uses an ATTINY84 microcontroller from
Atmel. It's configured to use the internal 8Mhz clock. The board
features and ISCP header, and power LED.

![Schematic](https://github.com/kippandrew/tiny-rc-serial/blob/master/tiny-rc-serial-schematic.png)

### Components ###

*Board*
- IC1 - Atmel ATTINY84A-SSU 14-SOIC SMD
- C1 - Capcaitor 0.1uF
- D1 - Diode SOD-323 SMD
- R1 - Resistor 330 ohm 1/4W 1206 SMD
- LED1 - LED red 1206 SMD
- JP1 - 0.1" Pitch 2x5
- JP2 - 0.1" pitch 2x3

Software
--------
