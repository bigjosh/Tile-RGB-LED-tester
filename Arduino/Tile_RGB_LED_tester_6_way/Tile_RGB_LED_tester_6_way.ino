

//#define delay(x) _delay_ms(x)   // We are steeling Arduino's timers, so delay() won't work

// Chips are in a matrix, drive the com pin high and the corresponding RGB pins low to light the led
// You can't light all at the same time, not enough current, so scan refresh them

#define CHIP_COUNT 6

  // Common pins - drive these HIGH to light the corresponding chip. They are in clockface order starting at noon. 

const uint8_t compins[] = {A0,A1,A2,A3,A4,A5};    // NOTE: Were are not using these as analog pins, just the A-names are handy and look nice in order.
                                                  

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

void setup() {

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

static inline void setRGB( uint8_t chip, uint8_t r, uint8_t g, uint8_t b ) {

    // All color values are inverted becuase we are running in a mode where we set the output HIGH on match

    colors_r[chip]= ~pgm_read_byte(&gamma8[r]);
    colors_g[chip]= ~pgm_read_byte(&gamma8[g]);
    colors_b[chip]= ~pgm_read_byte(&gamma8[b]);
  
}

static inline void setAllRGB( uint8_t r, uint8_t g, uint8_t b ) {
  for(int i=0;i<CHIP_COUNT;i++) {
    setRGB( i ,   r, g , b );
  }
}


static void setHSB( uint8_t chip, uint8_t inHue, uint8_t inSaturation, uint8_t inBrightness ) {

    uint8_t r;
    uint8_t g;
    uint8_t b;

    if (inSaturation == 0)
    {
        // achromatic (grey)
        r =g = b= inBrightness;
    }
    else
    {
        unsigned int scaledHue = (inHue * 6);
        unsigned int sector = scaledHue >> 8; // sector 0 to 5 around the color wheel
        unsigned int offsetInSector = scaledHue - (sector << 8);  // position within the sector         
        unsigned int p = (inBrightness * ( 255 - inSaturation )) >> 8;
        unsigned int q = (inBrightness * ( 255 - ((inSaturation * offsetInSector) >> 8) )) >> 8;
        unsigned int t = (inBrightness * ( 255 - ((inSaturation * ( 255 - offsetInSector )) >> 8) )) >> 8;

        switch( sector ) {
        case 0:
            r = inBrightness;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = inBrightness;
            b = p;
            break;
        case 2:
            r = p;
            g = inBrightness;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = inBrightness;
            break;
        case 4:
            r = t;
            g = p;
            b = inBrightness;
            break;
        default:    // case 5:
            r = inBrightness;
            g = p;
            b = q;
            break;
        }
    }

    setRGB( chip , r , g , b );
}

static void setAllHSB( uint8_t inHue, uint8_t inSaturation, uint8_t inBrightness ) {

  for( int i=0; i< CHIP_COUNT ; i++ ) {

    setHSB( i , inHue, inSaturation, inBrightness );
  }
}

// single breath, 255 steps in full cycle

uint8_t breathe( uint8_t step ) {

      // From: http://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
      // Modified slightly to start and end with LED off
      return ( ( (exp(cos( PI + ( ((step/255.0) * (2*PI) )))) - 0.36787944)*108.0));
  
}


void loop() {
  // put your main code here, to run repeatedly:

  setAllRGB(0,0,0);     // CLS
    
  // Flip though fundemental colors, full screen,  1 seconds each

  setAllRGB(  255, 0 , 0 );
  delay(1000);
  setAllRGB(  0 ,255, 0  );
  delay(1000);
  setAllRGB(  0, 0 , 255 );
  delay(1000);

  // Next a breath in each RGB...

  for( uint8_t step=1; step<250; step++) {
    setAllRGB( breathe( step) , 0 , 0  );
    delay(10); 
  }

  setAllRGB( 0 , 0 , 0 );
  delay(100); 
  
  for( uint8_t step=1; step<250; step++) {
    setAllRGB( 0 , breathe( step) , 0  );
    delay(10); 
  }


  setAllRGB( 0 , 0 , 0 );
  delay(100); 
  
  
  for( uint8_t step=1; step<250; step++) {
    setAllRGB( 0 , 0 , breathe( step)   );
    delay(10); 
  }


  setAllRGB( 0 , 0 , 0 );
  delay(100); 


  // Now a nice HSB color Wheel

  for( int l=0; l<CHIP_COUNT; l++ ) {
    for( int h=0; h<255; h++ ) {
  
      setAllHSB( h , 255 , 255 );
      delay(10);
  
      
    }
  }

  // Take her for a spin!!


  setAllRGB(0,0,0);     // CLS


  for( uint8_t d=100; d>0; d-=5) {

    for( uint8_t chip=0; chip<CHIP_COUNT; chip++ ) {

      setRGB( chip , 0 , 255 , 0 );
      delay(d);
      setRGB( chip , 0 , 0  , 0 );

    }    
  }

  
  // Let's try a smooth rainbow interpolated spin...


  setAllRGB(0,0,0);     // CLS

  for( uint8_t d=0; d<20; d++) {

    for( int a=0; a<255; a++ ) {     // Angle steps 

      for( uint8_t chip=0; chip<CHIP_COUNT; chip++ ) {

        uint8_t h = (a + (( 256 * chip)/CHIP_COUNT)) & 255;

        setHSB( chip ,  h , 255 , 255 );
        
      }

      delay(2);
      
        
    }    
    
  }


}
