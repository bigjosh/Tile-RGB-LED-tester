# Tile-RGB-LED-tester
A board for testing RGB LEDs on the Move32 tile platorm

###Features
* Each LED gets a full 24 bit RGB color
* Uses 6 pins total for 3 RGB LEDs
* All color setting consists of simple async writes to RAM
* R,G, or B of a single pixel can be set in a single instruction!
* With careful pin layout, the refresh ISR can be as short as 12 instructions
* LEDs arealways  refreshed at high rate, allowing resistor free operation

###Current Limitations
* Built using all Arduino code, so glitchs in timing ISR and poor performance
* Inline current limiting resistors are sized toprevent blowout, but that means brightness is limited to 1/3 maximum
* Runs on 5V supply- a boost inductor will be needed for green and blue ot run off 3V 
* Button is soldered on in wrong orientation (I had 50% shot!)


![alt text](2017-02-06%2015.04.12.jpg)


