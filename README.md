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
Atmel. The board features and ISCP programming header, decoupling caps
and power indicator LED.

The board accepts a +5V source, and can provide power to the receiver. 

![Schematic](https://raw.github.com/kippandrew/tiny-rc-serial/master/tiny-rc-serial-schematic.png)

![Board](https://raw.github.com/kippandrew/tiny-rc-serial/master/tiny-rc-serial-board.png)

### Components ###

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

The tiny RC serial interface runs and ATTINY Arduino core
(instructions on how to use the ATTINY Arduino core can be found
[here](http://hlt.media.mit.edu/?p=1695)). To program the board you
will need and AVR ISP programmer such as the [Pocket AVR
Programmer](https://www.sparkfun.com/products/9825) from SparkFun. It
is also possible to using a standard Arduino as an ISP programmer.

When programming the tiny RC serial interface for the first time, be
sure to burn the bootloader, and set the fuse bits for 8Mhz clock. The
default for the ATTINY84 is 1Mhz.

Instructions
------------

By default the tiny RC serial interface will output a 16 byte serial
packet every 100ms. The first 4 bytes are a header used to identify
the start of the packet. The packet header is 4 bytes with a value 0xFF.
Each channel follows the packet header encoded in two bytes.

Example Packet:

    --------------------------------------------------------------------------------------------------------------------------
    | header (4 bytes)       | CH1 (2 bytes) | CH2 (2 bytes) | CH3 (2 bytes) | CH4 (2 bytes) | CH5 (2 bytes) | CH6 (2 bytes) |
    --------------------------------------------------------------------------------------------------------------------------
    | 0xFF, 0xFF, 0xFF, 0xFF | 0x00, 0x01    | 0x02, 0x03    | 0x04, 0x05    | 0x06, 0x07    | 0x06, 0x07    | 0x08, 0x0A    |
    --------------------------------------------------------------------------------------------------------------------------

Example reading the packet from Arduino (Mega):

    uint8_t packetHeader[4] = {
      0xFF, 0xFF, 0xFF, 0xFF};

    void setup() {
      // initialize serial
      Serial.begin(115200); 
      Serial1.begin(9600);
    }

    boolean readPacketHeader(uint8_t *header, size_t headerLength, uint32_t timeout) {
      byte index = 0;
      byte b = 0;
      uint32_t startTime = millis();
      while (millis() - startTime < timeout) {
        if (index >= headerLength) {
          return true;
        }
        if (Serial1.available() > 0) {
          b = Serial1.read();
          if (b == header[index]) {
            index++;
          } 
          else {
            index = 0;
          }
        }
      }
      // timeout
      return false;
    }

    void loop() {
      static uint16_t channels[6];
      byte len;
      uint8_t buf[2];

      if (readPacketHeader(packetHeader, 4, 10)) {
        // for each channel
        for (int ch = 0; ch < 6; ch++) {
          // Read two bytes
          len = Serial1.readBytes((char*)buf, 2);
          if (len >= 2) {
            memcpy(&channels[ch], buf, sizeof(uint16_t));
          }
        }
      }
      
      for (int ch = 0; ch < 6; ch++) {
        Serial.print("ch ");
        Serial.print(ch + 1, DEC);
        Serial.print(" ");
        Serial.print(channels[ch]);
        Serial.print(" ");
      }
      
      Serial.println();
    }
