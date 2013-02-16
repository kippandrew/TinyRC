// 
// Copyright (C) 2012 Andy Kipp <kipp.andrew@gmail.com> 
// 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// Reference: ATMEL ATTINY84 / ARDUINO
//
//                           +-\/-+
//                     VCC  1|    |14  GND
//        TX - (D 10)  PB0  2|    |13  AREF (D  0) - CH1
//        RX - (D  9)  PB1  3|    |12  PA1  (D  1) - CH2
//                     PB3  4|    |11  PA2  (D  2) - CH3
//             (D  8)  PB2  5|    |10  PA3  (D  3) - CH4
//             (D  7)  PA7  6|    |9   PA4  (D  4) - CH5
//             (D  6)  PA6  7|    |8   PA5  (D  5) - CH6
//                           +----+

#define _SS_PCINT0_DISABLE

#include <CustomSoftwareSerial.h>

#define RECEIVER_CH1 0
#define RECEIVER_CH2 1
#define RECEIVER_CH3 2
#define RECEIVER_CH4 3
#define RECEIVER_CH5 4
#define RECEIVER_CH6 5

#define RECEIVER_CH1_IN_PIN 0 // pin 13
#define RECEIVER_CH2_IN_PIN 1  // pin 12
#define RECEIVER_CH3_IN_PIN 2  // pin 11
#define RECEIVER_CH4_IN_PIN 3  // pin 10
#define RECEIVER_CH5_IN_PIN 4  // pin 9
#define RECEIVER_CH6_IN_PIN 5  // pin 8

#define SERIAL_TX_PIN 10 // pin 2
#define SERIAL_RX_PIN 9  // pin 3

#define RECEIVER_CH1_FLAG 1
#define RECEIVER_CH2_FLAG 2
#define RECEIVER_CH3_FLAG 3
#define RECEIVER_CH4_FLAG 4
#define RECEIVER_CH5_FLAG 5
#define RECEIVER_CH6_FLAG 6

#define NUMBER_RECEIVER_CHANNELS 6

#define STARTUP_TIMER 500

#define OUTPUT_TIMER 100 

//#define DEBUG_SERIAL_OUTPUT 

// serial packet header
const uint8_t serialPacketHeader[4] = {0xFF, 0xFF, 0xFF, 0xFF};

// define receiver_interrupt struct
struct receiver_interrupt_t {
    uint8_t  pin;
    uint8_t  channel;
    uint8_t  bitMask;
};

// array of receiver_interrupt structs for each channel
volatile receiver_interrupt_t receiver_interrupts[NUMBER_RECEIVER_CHANNELS];

// receiver channel update flag 
volatile uint8_t bUpdateFlagsShared;

// receiver elasped time for each receiver channel
volatile uint16_t unReceiverShared[NUMBER_RECEIVER_CHANNELS];

// receiver start time for each receiver channel
volatile uint32_t ulReceiverStart[NUMBER_RECEIVER_CHANNELS];

// serial output timer
volatile static uint32_t ulOutputTimer = 0;

// serial interface
SoftwareSerial serial(SERIAL_RX_PIN, SERIAL_TX_PIN); // RX, TX

/**
 * setup a receiver_interrupt for the given channel on the given pin
 */
void setupReceiverInterrupt(uint8_t pin, uint8_t channel) {
  
  volatile receiver_interrupt_t *inrpt;
  
  // Get interrupt
  inrpt = &receiver_interrupts[channel];

  // Get bit mask for pin
  uint8_t bitMask = digitalPinToBitMask(pin);
  
  // Set interrupt data
  inrpt->bitMask = bitMask;
  inrpt->pin = pin;
  inrpt->channel = channel;
  
  // Enable pin change interrupt
  GIMSK |= (1<<PCIE0);
  if ( (PCMSK0 & bitMask) == 0 )
  {
    PCMSK0 |= bitMask;
  }
}

/**
 * handle interrupt for the given pin and channel
 */
void handleReceiverInterrupt(uint8_t pin, uint8_t channel) {
  // if the pin is high, its a rising edge of the signal pulse, so lets record its value
  if(digitalRead(pin) == HIGH)
  { 
    ulReceiverStart[channel] = micros();
  }
  else
  {
    // else it must be a falling edge, so lets get the time and subtract the time of the rising edge
    // this gives use the time between the rising and falling edges i.e. the pulse duration.
    unReceiverShared[channel] = (uint16_t)(micros() - ulReceiverStart[channel]);
    // set the channel flag to indicate that a new channel signal has been received
    uint8_t flag = channel + 1;
    bUpdateFlagsShared |= flag;
  }
}

/**
 * arduino setup
 */
void setup()
{
  // Open serial communications and wait for port to open
  serial.begin(9600);
  
  // setup receiver interrupts
  setupReceiverInterrupt(RECEIVER_CH1_IN_PIN, RECEIVER_CH1);
  setupReceiverInterrupt(RECEIVER_CH2_IN_PIN, RECEIVER_CH2);
  setupReceiverInterrupt(RECEIVER_CH3_IN_PIN, RECEIVER_CH3);
  setupReceiverInterrupt(RECEIVER_CH4_IN_PIN, RECEIVER_CH4);
  setupReceiverInterrupt(RECEIVER_CH5_IN_PIN, RECEIVER_CH5);
  setupReceiverInterrupt(RECEIVER_CH6_IN_PIN, RECEIVER_CH6);
  sei();
 
  delay(STARTUP_TIMER);
}

/**
 * arduino loop
 */
void loop()
{
  static uint8_t bUpdateFlags;
  static uint16_t unReceiver[NUMBER_RECEIVER_CHANNELS];
  uint8_t buf[sizeof(uint16_t)];
 
  // check shared update flags to see if any channels have a new signal
  if(bUpdateFlagsShared)
  {
    noInterrupts(); // turn interrupts off quickly while we take local copies of the shared variables

    // take a local copy of which channels were updated in case we need to use this in the rest of loop
    bUpdateFlags = bUpdateFlagsShared;

    // check for updated channels
    if(bUpdateFlags & RECEIVER_CH1_FLAG)
    {
      unReceiver[RECEIVER_CH1] = unReceiverShared[RECEIVER_CH1];
    }
    if(bUpdateFlags & RECEIVER_CH2_FLAG)
    {
      unReceiver[RECEIVER_CH2] = unReceiverShared[RECEIVER_CH2];
    }
    if(bUpdateFlags & RECEIVER_CH3_FLAG)
    {
      unReceiver[RECEIVER_CH3] = unReceiverShared[RECEIVER_CH3];
    }
    if(bUpdateFlags & RECEIVER_CH4_FLAG)
    {
      unReceiver[RECEIVER_CH4] = unReceiverShared[RECEIVER_CH4];
    }
    if(bUpdateFlags & RECEIVER_CH5_FLAG)
    {
      unReceiver[RECEIVER_CH5] = unReceiverShared[RECEIVER_CH5];
    }
    if(bUpdateFlags & RECEIVER_CH6_FLAG)
    {
      unReceiver[RECEIVER_CH6] = unReceiverShared[RECEIVER_CH6];
    }
    
    // clear shared copy of updated flags as we have already taken the updates
    bUpdateFlagsShared = 0;

    interrupts(); // turn interrupts back on
  } 

#ifdef DEBUG_SERIAL_OUTPUT 

  serial.write(0xFE);
  serial.write(0x01);
  
  serial.print(unReceiver[RECEIVER_CH1], DEC);
  serial.print(" ");
  serial.print(unReceiver[RECEIVER_CH2], DEC);
  serial.print(" ");
  serial.print(unReceiver[RECEIVER_CH3], DEC);
  serial.print(" ");
  serial.print(unReceiver[RECEIVER_CH4], DEC);
  serial.print(" ");
  serial.print(unReceiver[RECEIVER_CH5], DEC);
  serial.print(" ");
  serial.print(unReceiver[RECEIVER_CH6], DEC);
  delay(100);
  
#else

  // only write to the serial stream after OUTPUT_TIMER milliseconds
  if ((millis() - ulOutputTimer) >= OUTPUT_TIMER) {
    
    // update output timer
    ulOutputTimer = millis();
 
    // write packet header 
    serial.write(serialPacketHeader, sizeof(serialPacketHeader));

    // write receiver output to serial
    memcpy(buf, &unReceiver[RECEIVER_CH1], sizeof(uint16_t));
    serial.write(buf, sizeof(uint16_t));
    memcpy(buf, &unReceiver[RECEIVER_CH2], sizeof(uint16_t));
    serial.write(buf, sizeof(uint16_t));
    memcpy(buf, &unReceiver[RECEIVER_CH3], sizeof(uint16_t));
    serial.write(buf, sizeof(uint16_t));
    memcpy(buf, &unReceiver[RECEIVER_CH4], sizeof(uint16_t));
    serial.write(buf, sizeof(uint16_t));
    memcpy(buf, &unReceiver[RECEIVER_CH5], sizeof(uint16_t));
    serial.write(buf, sizeof(uint16_t));
    memcpy(buf, &unReceiver[RECEIVER_CH6], sizeof(uint16_t));
    serial.write(buf, sizeof(uint16_t));

  }
   
#endif

}

/**
 * interrupt service routine for pin change interrupt (PORTA)
 */
ISR( PCINT0_vect )
{
  volatile static uint8_t prevPinState;
  
  uint8_t currentPinState = PINA & PCMSK0;
  uint8_t bitMask = currentPinState ^ prevPinState;

  volatile receiver_interrupt_t *inrpt;
  
  inrpt = receiver_interrupts;
  for ( uint8_t i=0; i < NUMBER_RECEIVER_CHANNELS; i++ ) {
    if ((inrpt->bitMask & bitMask)){
      handleReceiverInterrupt(inrpt->pin, inrpt->channel);
      break;
    }
    ++inrpt;
  }
  
  prevPinState = currentPinState;
}
