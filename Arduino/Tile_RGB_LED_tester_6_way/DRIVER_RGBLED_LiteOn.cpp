#include <Arduino.h>

// This library supports a test board with 6 LiteOn RGB Leds connected to an Arduino Micro
// The R, G, and B pins for all the chips are tied together and connected to pins 10,11, and 12 respectively
// The common selects for each chip are connected to pins A0-A5. These select pins are driven HIGH to power each chip individually

// The software uses Timer1 to drive the RGB pins with a PWM signal generated in hardware.
// On each PWM cycle, the select is advanced to the next pin to mulitplex refresh all the chips at about 100Hz.

// The selected pins can be changed pretty easily, but the R,G, and B pins are tied to the OC1x timer outputs so can not be moved to other pins.
// This straegy depends on Timer1 having 3 output compare registers so would need to be changed to work on other chips that have fewer.

// Note that it seems like I need to push the reset button on my Arduino Micro to get it to accept a program download.

#if !defined(__AVR_ATmega32U4__)
	#error This LiteOn library is hardcoded for an Arduino Micro board only!
#endif

#include "DRIVER_RGBLED.h"

// Common pins - drive these HIGH to light the corresponding chip. They are in clockface order starting at noon.                                                  
const uint8_t compins[CHIP_COUNT] = {A0,A1,A2,A3,A4,A5};    // NOTE: These are the Arduino pin numbers as used in digitalWrite()

  // RGB pins. Drive LOW to light the corresponding color. 
  // We got lucky that they all land on one timer, and it is not used by arduino, and they are all next to each other on the Micro pinout!

  // Red    -> Pin 9 (OC1A)
  // Green -> Pin 10 (OC1B)
  // Blue  -> Pin 11 (OC1C)


#define BUTTON_PIN 7      // Must be interrupt capable

volatile uint8_t colors_r[CHIP_COUNT];
volatile uint8_t colors_g[CHIP_COUNT];
volatile uint8_t colors_b[CHIP_COUNT];

// This runs continuously in the background to refresh the chips.
// Scheduled so that it runs at the END of the PWM cycle, so that each chip gets a fair shake 

// TODO: Increease TOP so we have more time to set the new color. Will envolve adding an offset to stored color. 

static uint8_t previousChip=0;
static uint8_t currentChip=0;
static uint8_t nextChip=0;
  
ISR(TIMER1_OVF_vect) {
 
  // When we get here, then we know all LED colors are off becuase we are running in WGM10 mode when is "clear on match"

  digitalWrite( compins[previousChip] , 0 );   // Drive comm select low so nothing will light until we say so...

  // Note that the OCRs are already programmed for the correct values because they got loaded last time we hit TOP
   
  digitalWrite( compins[currentChip] , 1 );   // Drive comm select high to light chip
  
  // Set the pwm outputs for each color on this chip  
  // These will get loaded next time we hit TOP.
  // Only bother setting the lower byte since we max out at 255
  
  OCR1AL = colors_r[nextChip];
  OCR1BL = colors_g[nextChip];
  OCR1CL = colors_b[nextChip];  

  previousChip=currentChip;
  currentChip=nextChip;
  nextChip=nextChip+1;   

  if (nextChip>=CHIP_COUNT) {
    nextChip=0;
  }
}

void setupTimer1(void) {

  // Preset outputs to off. 

  OCR1A = 0x00ff;
  OCR1B = 0x00ff;
  OCR1C = 0x00ff;

  // Set COM to (1 0) Clear OCnA/OCnB/OCnC on compare match, set OCnA/OCnB/OCnC at TOP
  // We do this so that outputs go to 0 on match, turning on the LED. 
  // This means that we need to ~invert the OCR from the brightness level.
  // We do this so the LED is turned OFF when we get the TOP. This measn that the LED is off when the TOP ISR is called. 
  // This gives the overflow ISR a chance to update everything. Much better to have the LED off if we glitch cause you can really see and LED that was off 
  // too long for a millisecond, but you can Definately see one that was on for a millisecond when it shouldn't have been. 
  
  // The overflow ISR might not get to run instantly when the TOP hits so we need this window of time when the LED is off. 
  // This is exacerabated by Arduino becuase they oftne grab interrupts for a long time, making us miss ours.

  // Set WGM to 5 0 1 0 1 Fast PWM, 8-bit 0x00FF TOP TOP
  
  TCCR1A = _BV( COM1A1)  |  _BV( COM1B1)  |_BV( COM1C1)  | _BV(WGM10) ;    // Set OCnA/OCnB/OCnC on compare match, clear OCnA/OCnB/OCnC at TOP

/*
  TCCR1B =  _BV( CS12)  | _BV( WGM12)  ;   // Prescaller /256 = 64Khz clock,  /TOP=256,  244Hz refresh, good enough?
  //https://www.google.com/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#q=16Mhz%2F256%2F256%3D
*/

  TCCR1B =  _BV( CS11)  | _BV( WGM12)  ;   // Prescaller /64 = 64Khz clock,  /TOP=256,  976Hz refresh around all 6 leds, good enough?
  // https://www.google.com/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#q=16Mhz/64/256

  TIMSK1 = _BV( TOIE1  );    // ISR on TOP

  DDRB |= _BV( 5 );   // Enable output on OC1A
  DDRB |= _BV( 6 );   // Enable output on OC1B
  DDRB |= _BV( 7 );   // Enable output on OC1C
}


void setupLeds(void) {
	  // Common pins output.  Default to low (off).
  for(int i=0; i<CHIP_COUNT;i++) {
    pinMode( compins[i] , OUTPUT );
  }

  setupTimer1();
}


// Table stolen from 
// https://learn.adafruit.com/led-tricks-gamma-correction/the-quick-fix
// But we are going to have to emperically make correct full tables for all 3 colors 

const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

void setRGB( uint8_t chip, uint8_t r, uint8_t g, uint8_t b ) {

    // All color values are inverted becuase we are running in a mode where we set the output HIGH on match

    colors_r[chip]= ~pgm_read_byte(&gamma8[r]);
    colors_g[chip]= ~pgm_read_byte(&gamma8[g]);
    colors_b[chip]= ~pgm_read_byte(&gamma8[b]);
  
}


