
#include "DRIVER_RGBLED.h"

#include "LIB_RGBLED.h"

void setup() {

  setupLeds();
  
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

  // Next take a breath in each RGB...
  
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


  // Detonate effect

  setAllRGB(0,0,0);     // CLS

  for( uint8_t d=200; d>0; d-= 10) {
      
      setAllRGB( 255 , 0, 0);
      delay(5);
      setAllRGB( 0, 0 , 0 );
      delay(d);
            
  }

  setAllRGB( 255 , 255, 255);

  delay(10);

  for(int b=255;b>0;b--) {
    setAllRGB( b , b , b );
    delay(2);
  }
  
// Rotating blue wave effect

  for( int a=0; a<100; a+=1 ) {     // Angle though 100 steps

    for( int w=0; w<100; w++) {   // Steps in each Wave 

      // Imagine the wave going to left to right, wavelength is 2. Each wave is a half a sin()

      for( int c=0; c<CHIP_COUNT; c++ ) {

        // Represent the pixels in polar cooridinates at radius 1
        // Rotate the pixel into position on the board, plus the addisional angle for the wave rotation

        // Compute the x position of the pixel Relative to the incoming wave (it is coming left to right on the x axis)
        // This x will be -1 to 1
        
        float x= sin( 
          2 * PI * (        
          
              (( (float) c ) /CHIP_COUNT)  +        // The angle of the chip around the board
              (( (float) a ) /100)                  // add in the dynamic rotating angle
            
          )        
        );

        // Next compute the intensity of the wave at this location
        // Clip at zero so we only get the positive side of the wave, the neg side will be black and be a nice break between crashing waves
        
        uint8_t b= max( 0 , sin( ( 2* PI * ( (w/100.0) + (x)  ))) ) * 255.0;

        setRGB( c , 0 , 0 , b );
        
      }

      delay(2);
      
    }

  }


  
}
