
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

  float rotatedAngle=0;             // Current rotated angle

  const int waveSteps = 100;        // Number of steps to take when passing the wave acorss the board for each angle step. 

  const int cycle_count = 10;      // Number of waves to cycle to get all the way round

  for( int a=0; a<cycle_count; a++ ) {     // Rotate wave aproach angle though 100 steps around 

    for( int w=0; w<waveSteps; w++) {     // Steps in each Wave cycle

      // Imagine the wave is always going to left to right (across X values), wavelength is 2 units. The pixles are all 1 unit from the center.
      
      for( int c=0; c<CHIP_COUNT; c++ ) {

        // Represent the pixels in polar cooridinates at radius 1
        // Rotate the pixel into position on the board, plus the addisional angle for the wave rotation

        // Compute the x position of the pixel Relative to the incoming wave (it is coming left to right on the x axis)
        // This x will be -1 to 1
        
        float x= sin( 
          
          2.0 * PI * (        
          
              (( (float) c ) /CHIP_COUNT)          // The angle of the chip around the board - when CHIP_COUNT is 6, then this ends up being every 30 degrees
                                    
          )        

          +rotatedAngle                        // add in the dynamic rotating angle

        );

        // Next compute the intensity of the wave at this X location (y doesn't matter since this is a planar wavefront)
        
        uint8_t b=  (
          
          -1.0 * cos( 
            
              ( 
                 2* PI *     // flip the COS so we start a -1 rather than 1 (we will later adjust up 1 so we start a 0 and peak at 2)

                  ( ((float) w )/ waveSteps )       // This is the actual moving wave value at x=0

              ) +x 

          )

          +1                                      // Normalize from [-1 to 1] to [0 to 2]. 
          
        ) * 127.0;   // Bring into our 8-bit color space

        setRGB( c , 0 , 0 , b );
        
      }


      delay(10);

      rotatedAngle += ( 2.0 * PI ) / ( 1.0 * waveSteps * cycle_count);
      
    }

  }

  
  
}
