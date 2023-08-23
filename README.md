## bleached
small/inexpensive 7-knob teensy midi controller. 

many thanks to [brian crabtree](https://vimeo.com/134631071) for sending me the original design & helping me add the teensy !!

![product image](pic.JPG)

## BOM

- [full BOM other than knobs](https://octopart.com/bom-tool/u6LfSTSI)
- [knobs from thonk (uk)](https://www.thonk.co.uk/shop/intellijel-black-knobs/) - [caps](https://www.thonk.co.uk/shop/sifam-caps/)

## Build

start with soldering all header pins to the underside of the teensy 3.2, reference the pic below for placement. the goal is to hit all of the power/ground pinouts along with the analog ins. solder short sides the two through-hole pins to the teensy. 

![pins image](pins.JPG)

getting the surface mount pins in the right place is the trickiest part of this project. start with those (one leg at a time), use tweezers, and test placement on the board with the through-hole headers.

the rest should be pretty easy, solder all the other headers to the teensy, solder the teensy to the board (on the top / stencil side of the board), and solder every leg of each of the pots. load firmware & test once everything is soldered (although, at this point there's kinda no going back) - you can test your pots by loading the [test_inputs](./firmware/test_inputs/test_inputs.ino) sketch onto the board, and looking at the serial monitor window (Tools -> Serial Monitor) while twiddling the knobs. the firmware is an arduino project for use with [teensyduino](https://www.pjrc.com/teensy/teensyduino.html). 

next, add the metal standoffs, shorter ones on the bottom, taller ones on top. then screw in the panels. if you want the pots extra stable you can add the nuts and washers that come with the potentiometers. then plop on yr favorite knobs & yr golden !

## Software
Fake 14 bit mode added, plus possibility to swap between 7 and 14 bits / 1 or 2 midi channel resolution.
(Teensy LC can only read 12 bits from pots, 3.2 can only read 13 bits. The result gets upscaled and passed as 14 bit.)

To change mode, set all pots maximum CW and connect device. This puts the device in a setup mode.
From there, the first pot sets bit mode. > 50% = 14 bit, < 50% 7 bit.
To save the change, move the second pot CCW. This will exit the setup mode and return to regular operation.
To cancel, power off device, move some knobs and reboot.

## FAQ

q1) i don't have a 5-pin header for the top end of the teensy.
a2) you should have a 3-pin header, which should be used with the 3 middle pins.

q2) i get an error compiling the firmware, `'usbMIDI' was not declared in this scope`.
a2) set the USB type to 'Serial + MIDI' on the Tools -> USB Type menu.
