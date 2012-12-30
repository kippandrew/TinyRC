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

![Schematic](https://raw.github.com/kippandrew/tiny-rc-serial/master/tiny-rc-serial-schematic.png)

### Components ###

**Board**

<table>
    <tr>
        <th>Part</th>
        <th>Description</th>
        <th>Part Number</th>
    </tr>
    <tr>
        <td>IC1</td>
        <td>Atmel ATTINY84A-SSU 14-SOIC SMD</td>
        <td>[DigiKey P/N: ATTINY84A-SSU-ND](http://www.digikey.com/product-detail/en/ATTINY84A-SSU/ATTINY84A-SSU-ND/3046522)</td>
    </tr>
    <tr>
    </tr>
</table>
- C1 - Capcaitor 0.1uF [DigiKey P/N: 399-4151-ND](http://www.digikey.com/product-detail/en/C315C104M5U5TA/399-4151-ND/817927)
- D1 - Diode SOD-323 SMD [DigiKey P/N: 568-5003-1-ND](http://www.digikey.com/product-detail/en/BAS316,115/568-5003-1-ND/2209919)
- R1 - Resistor 330 ohm 1/4W 1206 SMD [DigiKey P/N: 311-330ERCT-ND](http://www.digikey.com/product-detail/en/RC1206JR-07330RL/311-330ERCT-ND/732226)
- LED1 - LED Red 1206 SMD [DigiKey P/N: 160-1167-1-ND](http://www.digikey.com/product-detail/en/LTST-C150CKT/160-1167-1-ND/269239)
- JP1 - 0.1" pitch Header 2x5 [DigiKey P/N: ]()
- JP2 - 0.1" pitch Header 2x3 [DigiKey P/N: ]()
- JP3 - JST PH Header (4) [DigiKey P/N: ]() 

Software
--------
