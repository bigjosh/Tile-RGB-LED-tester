# Tile-RGB-LED-tester
A board for testing RGB LEDs on the Move32 [tile platorm](https://www.media.mit.edu/projects/automatiles/overview/).

###Features
* Each LED gets an independant full 24 bit RGB color
* Uses 9 pins total for 6 RGB LEDs
* All color setting consists of simple async writes to RAM bytes
* Color PWM done in hardware, pixel multiplex refresh is the only software ISR
* With careful pin layout, the pixel refresh ISR can be as short as 12 instructions
* LEDs are constantly refreshed, paving the way for resistor-free operation

###Video!

Here is the latest 6-LED tester in action!

https://www.youtube.com/watch?v=pqBiF4joLiI

![alt text](2017-02-06%2015.04.12.jpg)


