//
// Copyright (C) 2012-2013 Andy Kipp <kipp.andrew@gmail.com> 
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

// Reference: ATMEL ATMEGA328
//
//                           +-\/-+
//             RESET  PC6   1|    |28  PC5  (A 5)
//         (D 0) RX   PD0   2|    |27  PC4  (A 4)
//         (D 1) TX   PD1   3|    |26  PC3  (A 3)
//             (D 2)  PD2   4|    |25  PC2  (A 2)
//             (D 3)  PD3   5|    |24  PC1  (A 1)
//             (D 4)  PD4   6|    |23  PC0  (A 0)
//                    VCC   7|    |22  GND
//                    GND   8|    |21  AREF
//             XTAL1  PB6   9|    |20  AVCC (A 0)
//             XTAL2  PB7  10|    |19  PB5  (D 13)
//             (D 5)  PD5  11|    |18  PB4  (D 12)
//             (D 6)  PD6  12|    |17  PB3  (D 11)
//             (D 7)  PD7  13|    |16  PB2  (D 10)
//             (D 8)  PD0  14|    |15  PB1  (D 9)
//                           +----+

#define RECEIVER_CH1 0
#define RECEIVER_CH2 1
#define RECEIVER_CH3 2
#define RECEIVER_CH4 3
#define RECEIVER_CH5 4
#define RECEIVER_CH6 5

#define RECEIVER_CH1_IN_PIN 0  // PC0 / A0 / PCINT8
#define RECEIVER_CH2_IN_PIN 1  // PC1 / A1 / PCINT9
#define RECEIVER_CH3_IN_PIN 2  // PC2 / A2 / PCINT10
#define RECEIVER_CH4_IN_PIN 3  // PC3 / A3 / PCINT11
#define RECEIVER_CH5_IN_PIN 4  // PC4 / A4 / PCINT12
#define RECEIVER_CH6_IN_PIN 5  // PC5 / A5 / PCINT13

#define RECEIVER_CH1_FLAG 1
#define RECEIVER_CH2_FLAG 2
#define RECEIVER_CH3_FLAG 3
#define RECEIVER_CH4_FLAG 4
#define RECEIVER_CH5_FLAG 5
#define RECEIVER_CH6_FLAG 6

#define NUMBER_RECEIVER_CHANNELS 6

#define RECEIVER_STATUS_WATCHDOG   1

#define STARTUP_TIMER 500
#define OUTPUT_TIMER 100
#define WATCHDOG_TIMER 500

//#define DEBUG_SERIAL_OUTPUT

// Serial packet header
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
volatile uint8_t shared_receiver_update_flags;

// receiver elasped time for each receiver channel
volatile uint16_t shared_receiver_timer[NUMBER_RECEIVER_CHANNELS];

// receiver start time for each receiver channel
volatile uint32_t shared_receiver_timer_start[NUMBER_RECEIVER_CHANNELS];

// watchdog timer
volatile uint32_t watchdog_timer = 0;

// receiver status flags
volatile uint8_t receiver_status_flags = 0;

// Serial output timer
volatile uint32_t output_timer = 0;

// Serial interface
//SoftwareSerial SoftwareSerial(SERIAL_RX_PIN, SERIAL_TX_PIN); // RX, TX

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
  //GIMSK |= (1<<PCIE);
  PCICR |= (1<<1);
  if ( (PCMSK1 & bitMask) == 0 )
  {
    PCMSK1 |= bitMask;
  }
}

/**
 * handle interrupt for the given pin and channel
 */
void handleReceiverInterrupt(uint8_t pin, uint8_t channel) {
  // if the pin is high, its a rising edge of the signal pulse, so lets record its value
  if(digitalRead(pin) == HIGH)
  {
    shared_receiver_timer_start[channel] = micros();
  }
  else
  {
    // else it must be a falling edge, so lets get the time and subtract the time of the rising edge
    // this gives use the time between the rising and falling edges i.e. the pulse duration.
    shared_receiver_timer[channel] = (uint16_t)(micros() - shared_receiver_timer_start[channel]);
    // set the channel flag to indicate that a new channel signal has been received
    uint8_t flag = channel + 1;
    shared_receiver_update_flags |= flag;
  }
}

/**
 * arduino setup
 */
void setup()
{
  // Open Serial communications and wait for port to open
  Serial.begin(9600);

  // setup receiver interrupts

  //setupReceiverInterrupt(analogInputToDigialPin(RECEIVER_CH1_IN_PIN), RECEIVER_CH1);
  setupReceiverInterrupt(analogInputToDigitalPin(RECEIVER_CH2_IN_PIN), RECEIVER_CH2);
  //setupReceiverInterrupt(RECEIVER_CH3_IN_PIN, RECEIVER_CH3);
  //setupReceiverInterrupt(RECEIVER_CH4_IN_PIN, RECEIVER_CH4);
  //setupReceiverInterrupt(RECEIVER_CH5_IN_PIN, RECEIVER_CH5);
  //setupReceiverInterrupt(RECEIVER_CH6_IN_PIN, RECEIVER_CH6);
  sei();

  delay(STARTUP_TIMER);
}

/**
 * arduino loop
 */
void loop()
{
  static uint8_t receiver_update_flags;
  static uint16_t receiver[NUMBER_RECEIVER_CHANNELS];
  uint8_t buf[sizeof(uint16_t)];

  // check shared update flags to see if any channels have a new signal
  if(shared_receiver_update_flags)
  {
    noInterrupts(); // turn interrupts off quickly while we take local copies of the shared variables

    // update watchdog timer
    watchdog_timer = millis();

    // take a local copy of which channels were updated in case we need to use this in the rest of loop
    receiver_update_flags = shared_receiver_update_flags;

    // check for updated channels
    if(receiver_update_flags & RECEIVER_CH1_FLAG)
    {
      receiver[RECEIVER_CH1] = shared_receiver_timer[RECEIVER_CH1];
    }
    if(receiver_update_flags & RECEIVER_CH2_FLAG)
    {
      receiver[RECEIVER_CH2] = shared_receiver_timer[RECEIVER_CH2];
    }
    if(receiver_update_flags & RECEIVER_CH3_FLAG)
    {
      receiver[RECEIVER_CH3] = shared_receiver_timer[RECEIVER_CH3];
    }
    if(receiver_update_flags & RECEIVER_CH4_FLAG)
    {
      receiver[RECEIVER_CH4] = shared_receiver_timer[RECEIVER_CH4];
    }
    if(receiver_update_flags & RECEIVER_CH5_FLAG)
    {
      receiver[RECEIVER_CH5] = shared_receiver_timer[RECEIVER_CH5];
    }
    if(receiver_update_flags & RECEIVER_CH6_FLAG)
    {
      receiver[RECEIVER_CH6] = shared_receiver_timer[RECEIVER_CH6];
    }

    // clear shared copy of updated flags as we have already taken the updates
    shared_receiver_update_flags = 0;

    interrupts(); // turn interrupts back on
  }

  // check watchdog timer
  if ((millis() - watchdog_timer) >= WATCHDOG_TIMER) {
    receiver_status_flags |= RECEIVER_STATUS_WATCHDOG;
  } else {
    receiver_status_flags &= ~RECEIVER_STATUS_WATCHDOG;
  }

#ifdef DEBUG_Serial.OUTPUT 

  Serial.write(0xFE);
  Serial.write(0x01);
  Serial.print(receiver_status_flags, HEX);
  Serial.print(" ");
  Serial.print(receiver[RECEIVER_CH1], DEC);
  Serial.print(" ");
  Serial.print(receiver[RECEIVER_CH2], DEC);
  Serial.print(" ");
  Serial.print(receiver[RECEIVER_CH3], DEC);
  Serial.print(" ");
  Serial.print(receiver[RECEIVER_CH4], DEC);
  Serial.print(" ");
  Serial.print(receiver[RECEIVER_CH5], DEC);
  Serial.print(" ");
  Serial.print(receiver[RECEIVER_CH6], DEC);
  delay(100);

#else

  // only write to the Serial.stream after OUTPUT_TIMER milliseconds
  if ((millis() - output_timer) >= OUTPUT_TIMER) {

    // update output timer
    output_timer = millis();

    // write packet header 
    Serial.write(serialPacketHeader, sizeof(serialPacketHeader));

    // write receiver output to serial
    memcpy(buf, &receiver[RECEIVER_CH1], sizeof(uint16_t));
    Serial.write(buf, sizeof(uint16_t));
    memcpy(buf, &receiver[RECEIVER_CH2], sizeof(uint16_t));
    Serial.write(buf, sizeof(uint16_t));
    memcpy(buf, &receiver[RECEIVER_CH3], sizeof(uint16_t));
    Serial.write(buf, sizeof(uint16_t));
    memcpy(buf, &receiver[RECEIVER_CH4], sizeof(uint16_t));
    Serial.write(buf, sizeof(uint16_t));
    memcpy(buf, &receiver[RECEIVER_CH5], sizeof(uint16_t));
    Serial.write(buf, sizeof(uint16_t));
    memcpy(buf, &receiver[RECEIVER_CH6], sizeof(uint16_t));
    Serial.write(buf, sizeof(uint16_t));

    // write receiver status flags 
    Serial.write(receiver_status_flags);

  }

#endif

}

/**
 * interrupt service routine for pin change interrupt (PORTA)
 */
ISR( PCINT1_vect )
{
  volatile static uint8_t prevPinState;

  uint8_t currentPinState = PORTC & PCMSK1;
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
