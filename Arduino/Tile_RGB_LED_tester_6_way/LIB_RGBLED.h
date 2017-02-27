// This lib contains some helpful RGB LED library functions
// It assumes that an RGB LED driver has already been included above it


void setHSB( uint8_t chip, uint8_t inHue, uint8_t inSaturation, uint8_t inBrightness );

void setAllRGB( uint8_t r, uint8_t g, uint8_t b );
void setAllHSB( uint8_t inHue, uint8_t inSaturation, uint8_t inBrightness );

uint8_t breathe( uint8_t step );
