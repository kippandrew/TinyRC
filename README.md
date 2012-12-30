tiny-rc-serial
==============

About
-----
The tiny RC serial interface is a open source hardware device for
converting servo signals from RC receivers (such as the Spektrum
AR6210) into a serial stream that can be read by a microcontroller
such as an Arduino. 

The tiny RC serial interface works by timing pulses received on up to
6 channels, and uses an ATTINY84 microcontroller to convert those
pulses into a serial packet stream.

Hardware
--------

The tiny RC serial interface uses an ATTINY84 microcontroller from
Atmel. It's configured to use the internal 8Mhz clock. The board
features and ISCP header, and power LED.

### Components ###

- IC1 
- C1
- D1
- R1
- LED1
- JP1
- JP2

Software
--------
