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
        <td>DigiKey P/N: <a href="http://www.digikey.com/product-detail/en/ATTINY84A-SSU/ATTINY84A-SSU-ND/3046522">ATTINY84A-SSU-ND</a></td>
    </tr>
    <tr>
        <td>C1</td>
        <td>Capcaitor 0.1uF</td>
        <td>DigiKey P/N: <a href="http://www.digikey.com/product-detail/en/C315C104M5U5TA/399-4151-ND/817927">399-4151-ND</a></td>
    </tr>
    <tr>
        <td>D1</td>
        <td>Diode SOD-323 SMD</td>
        <td>DigiKey P/N: <a href="http://www.digikey.com/product-detail/en/BAS316,115/568-5003-1-ND/2209919">568-5003-1-ND</a></td>
    </tr>
    <tr>
        <td>R1</td>
        <td>Resistor 330 ohm 1/4W 1206 SMD</td>
        <td>DigiKey P/N: <a href="http://www.digikey.com/product-detail/en/RC1206JR-07330RL/311-330ERCT-ND/732226">311-330ERCT-ND</a></td>
    </tr>
    <tr>
        <td>LED1</td>
        <td>LED Red 1206 SMD</td>
        <td>DigiKey P/N: <a href="http://www.digikey.com/product-detail/en/LTST-C150CKT/160-1167-1-ND/269239">160-1167-1-ND</a></td>
    </tr>
    <tr>
        <td>JP1</td>
        <td>0.1" pitch Header 2x5</td>
        <td>DigiKey P/N: <a href="http://www.digikey.com/product-detail/en/67997-410HLF/609-3243-ND/1878475">609-3243-ND</a></td>
    </tr>
    <tr>
        <td>JP2</td>
        <td>0.1" pitch Header 2x3</td>
        <td>DigiKey P/N: <a href="http://www.digikey.com/product-detail/en/77313-422R06LF/609-4517-ND/2665635">609-4517-ND</a></td>
    </tr>
    <tr>
        <td>JP3</td>
        <td>JST PH Header (4) </td>
        <td>DigiKey P/N: <a href="http://www.digikey.com/scripts/DkSearch/dksus.dll?WT.z_header=search_go&lang=en&keywords=455-1706-ND&x=0&y=0&cur=USD">455-1706-ND</a></td>
    </tr>
</table>

Software
--------
