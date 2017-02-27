// This is the generic RGB LED driver template
// You must copy one of the concrete RGB LED implementations .c files into the sketch directory to 
// implement these functions.

// This would be more elegant if we could make each driver a library and you pick which one to 
// include, but no good way to do this on Arduino....

// TODO: Current gamma table is generic and the same for all colors. Need to imperically make one that matches
//       the actual chips and driving scheme for each color. 

// Number of LED chips available
#define CHIP_COUNT 6

// Must be called once at startup
// Sets all LEDs to black (off)
void setupLeds(void);

// Change the color of one of the LEDs
void setRGB( uint8_t chip, uint8_t r, uint8_t g, uint8_t b );



