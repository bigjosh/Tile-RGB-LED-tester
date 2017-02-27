#include <Arduino.h>

#include "DRIVER_RGBLED.h"
#include "LIB_RGBLED.h"

void setAllRGB( uint8_t r, uint8_t g, uint8_t b) {
  for(int i=0;i<CHIP_COUNT;i++) {
    setRGB( i ,   r, g , b );
  }
}

void setHSB( uint8_t chip, uint8_t inHue, uint8_t inSaturation, uint8_t inBrightness ) {

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

void setAllHSB( uint8_t inHue, uint8_t inSaturation, uint8_t inBrightness ) {

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

