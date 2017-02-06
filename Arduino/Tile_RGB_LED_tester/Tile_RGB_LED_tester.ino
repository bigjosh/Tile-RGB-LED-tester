
#include <TimerOne.h>

// Chips are in a matrix, drive the com pin high and the corresponding RGB pins low to light the led
// You can't light all at the same time, not enough current, so scan refresh them

#define CHIP_COUNT 3

  // Common pins - drive these HIGH to light the corresponding chip. They are in clockface order starting at noon. 

const uint8_t compins[] = {10,11,12};

  // RGB pins. Drive low to light the corresponding color. These are all analogWrite() pins.

#define PIN_R 3
#define PIN_G 5
#define PIN_B 6


#define BUTTON_PIN 7      // Must be interrupt capable

volatile uint8_t colors_r[] = { 0,0,0};    
volatile uint8_t colors_g[] = { 0,0,0};    
volatile uint8_t colors_b[] = { 0,0,0};    

// This runs continuously in the background to refresh the chips.

void refreshLedsISR(void) {
  
  static uint8_t currentChip=0;

  digitalWrite( compins[currentChip] , 0 );   // Drive comm low to turn off chip

  currentChip++;
  
  if (currentChip>=CHIP_COUNT) {

    currentChip=0;
  }

  // Set the pwm outputs for each color on this chip  
  analogWrite( PIN_R , colors_r[currentChip]);
  analogWrite( PIN_G , colors_g[currentChip]);
  analogWrite( PIN_B , colors_b[currentChip]);

  digitalWrite( compins[currentChip] , 1 );   // Drive comm high to light chip

}


void setup() {

  // Common pins output.  Default to low (off).
  for(int i=0; i<CHIP_COUNT;i++) {
    pinMode( compins[i] , OUTPUT );
  }

/*
  // RGB pins output.  Set to high (OFF)
  for(int i=0; i<CHIP_COUNT;i++) {
    digitalWrite( rgbpins[i], 1 );
    pinMode( rgbpins[i] , OUTPUT );
  }
  */
  Timer1.initialize(5000);       // refresh interval in 5ms. Imprtially determined to just avoid flicker.
  Timer1.attachInterrupt(refreshLedsISR); 
  
}

static inline void setRGB( uint8_t chip, uint8_t r, uint8_t g, uint8_t b ) {

    // All color values are inverted becuase color pins are active low...

    colors_r[chip]= ~r;
    colors_g[chip]= ~g;
    colors_b[chip]= ~b;
  
}

static inline void setAllRGB( uint8_t r, uint8_t g, uint8_t b ) {
  setRGB( 0 ,   r, g , b );
  setRGB( 1 ,   r, g , b );
  setRGB( 2 ,   r, g , b );
}


static void setAllHSB( uint8_t inHue, uint8_t inSaturation, uint8_t inBrightness ) {

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

    setAllRGB( r , g , b );
}

// single breath, 255 steps in full cycle

uint8_t breathe( uint8_t step ) {

      // From: http://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
      // Modified slightly to start and end with LED off
      return ( ( (exp(cos( PI + ( ((step/255.0) * (2*PI) )))) - 0.36787944)*108.0));
  
}


void loop() {
  // put your main code here, to run repeatedly:

  // Flip though fun colors, full screen,  1 seconds each

  setAllRGB(  255, 255 , 0 );
  delay(1000);
  setAllRGB(  255, 0 , 255 );
  delay(1000);
  setAllRGB(  0, 255 , 255 );
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

  for( int l=0; l<3; l++ ) {
    for( int h=0; h<255; h++ ) {
  
      setAllHSB( h , 255 , 255 );
      delay(10);
  
      
    }
  }

  // Take her for a spin!!


  setAllRGB(0,0,0);     // CLS


  for( uint8_t d=100; d>0; d--) {

    for( uint8_t chip=0; chip<CHIP_COUNT; chip++ ) {

      setRGB( chip , 0 , 255 , 0 );
      delay(d);
      setRGB( chip , 0 , 0  , 0 );

    }
  }

}
