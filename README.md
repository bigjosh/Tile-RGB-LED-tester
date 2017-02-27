# Tile-RGB-LED-tester
A board for testing RGB LEDs on the [Move38](http://www.move38.com) [tile platorm](https://www.media.mit.edu/projects/automatiles/overview/).

###Features
* Each LED gets an independant full 24 bit RGB color
* Uses 6 pins total for 3 RGB LEDs
* All color setting consists of simple async writes to RAM bytes
* R,G, or B of a single pixel can be set in a single instruction!
* Color PWM done in hardware, pixel multiplex refresh is the only software ISR
* With careful pin layout, the pixel refresh ISR can be as short as 12 instructions
* LEDs are constantly refreshed, paving the way for resistor-free operation

###Current Limitations
* Built using all Arduino code, so there are glitches in PWM timing and very poor performance
* Inline current limiting resistors are sized to prevent blowout (no one wants to rework 060x060's!), but that means brightness is limited to less than 1/3 theoretical maximum
* Runs on 5V supply- a boost inductor will be needed for green and blue ot run off 3V 
* Button is in wrong orientation (I had 50% shot!) so doesn't work 

![alt text](2017-02-06%2015.04.12.jpg)


