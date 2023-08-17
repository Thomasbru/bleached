/* Sonoclast pd knobs for TeensyLC
 
  * Modified by andrew for bleached (Teensy 3.2) 02/22/2019
 

   Copyright March 2019 - havencking@gmail.com

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.

   The Sonoclast pd knobs is a MIDI CC controller designed particularly
   for use with pure data.  However it could be used to control any
   software that recognizes MIDI CC messages.  Customize the Control
   Channels below if you like.

   Compiling instructions:

   Follow these instructions to install the Arduino software and Tensyduino:
   https://www.pjrc.com/teensy/td_download.html

   In the Arduino software be sure to select the following:
   Tools > Board > Teensy3.2
   Tools > USB Type > Serial + MIDI
*/

#include <EEPROM.h> // for mode to persist between read/writes

// Map MIDI CC channels to knobs numbered left to right, defined as pairs.
// First item MSB, second LSB
#define CC01  {102, 103}
#define CC02  {104, 105}
#define CC03  {106, 107}
#define CC04  {108, 109}
#define CC05  {110, 111}
#define CC06  {112, 113}
#define CC07  {114, 115}

// Map the TeensyLC pins to each potentiometer numbered left to right.
#define POT01 0
#define POT02 1
#define POT03 2
#define POT04 3
#define POT05 4
#define POT06 5
#define POT07 6

// Use this MIDI channel.
#define MIDI_CHANNEL 1

// modes
#define EEPADDRESS 0

bool setup_mode = false;
bool bit_mode;


// Send MIDI CC messages for all 7 knobs after the main loop runs this many times.
// This prevents having to twiddle the knobs to update the receiving end.
// 10,000 loops is roughly 10 seconds.
//#define LOOPS_PER_REFRESH 10000

// potentiometer read parameters
#define POT_BIT_RES         10 // 16 works, 7-16 is valid
#define POT_NUM_READS       32 // 32 works

// Track the knob state.
uint16_t prev_pot_val[7] = {0xffff,
                             0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
                            };

// an array of potentiometer pins
uint8_t pot[7] = {POT01,
                   POT02, POT03, POT04, POT05, POT06, POT07
                  };

// an array of CC numbers
uint8_t cc[7][2] = { CC01,
                  CC02, CC03, CC04, CC05, CC06, CC07
                 };

// Prevent jitter when reading the potentiometers.
// Higher value is less chance of jitter but also less precision.
const uint8_t nbrhd = 5;

// Count the number of main loops.
uint16_t loop_count = 0;

void setup() {
  // serial monitoring for debugging
  //Serial.begin(38400);
  Serial.begin(9600);
  // potentiometers
  
  // READ MODE FROM EEPROM
  bit_mode = EEPROM.read(EEPADDRESS);

  analogReadResolution(POT_BIT_RES);
  analogReadAveraging(POT_NUM_READS);
  
  // setup mode
  check_for_setup();
  if (setup_mode == true){
    setup_function();
  }
}

void loop() {
  // Read each knob, and send MIDI CC only if the value changed.
  for (uint8_t i = 0; i < 7; i++) {
    uint16_t pot_val = analogRead(pot[i]);
    if ((pot_val < prev_pot_val[i] - nbrhd) ||
        (pot_val > prev_pot_val[i] + nbrhd)) {
      usbMIDI.sendControlChange(cc[i][0], pot_val >> (POT_BIT_RES - 7), MIDI_CHANNEL); // MSB
      if (bit_mode == true){
        usbMIDI.sendControlChange(cc[i][1], pot_val & 127 , MIDI_CHANNEL); // Sends LSB if in 14 bit mode
      }
      prev_pot_val[i] = pot_val;
    }
  }
  // MIDI Controllers should discard incoming MIDI messages.
  // (reference: https://www.pjrc.com/teensy/td_midi.html)
  while (usbMIDI.read()) { ;; }

  // Periodically send MIDI CC for every knob so that the receiving end matches the knobs
  // even when changing pure data patches.
//  if (loop_count > LOOPS_PER_REFRESH) {
//    for (uint8_t i = 0; i < 7; i++) {
//      usbMIDI.sendControlChange(cc[i], analogRead(pot[i]) >> (POT_BIT_RES - 7), MIDI_CHANNEL);
//    }
//    loop_count = 0;
//  }
//  loop_count++;
}


void check_for_setup(){
  // checks if all knobs are CW. If so, enters setup_mode
  for (uint8_t i = 0; i < 7; i++) {
    uint16_t pot_val = analogRead(pot[i]);
    if (pot_val < 1020) {
      setup_mode = false;
      break;
    }
    else{
      setup_mode = true; 
    }
  }
}

void setup_function(){
  // In setup mode:
  // First pot sets byte mode: CW half (towards full) = 14 bit MIDI, CCW half (towards 0) = 8 bit midi
  // Second pot CCW saves mode. To cancel, reboot Teensy.

    while (setup_mode == true) {
    for (uint8_t i = 0; i < 2; i++){
      uint16_t pot_val = analogRead(pot[i]);
      if (i == 0 && pot_val >= 500 && pot_val < 1020){
        bit_mode = true;
        }
      else if (i == 0 && pot_val < 500){
        bit_mode = false;
      }
      if (i == 1 and pot_val < 500){
        // save to eeprom
        EEPROM.write(EEPADDRESS, bit_mode);
        setup_mode = false;
      }
    }
    Serial.println(bit_mode);
    Serial.println(setup_mode);
    delay(500);
  }
}
