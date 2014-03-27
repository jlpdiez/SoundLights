// #############################################################################
// 
// # Name       : SoundLights
// # Version    : 1.0

// # Author     : Juan L. Perez Diez <ender.vs.melkor at gmail>
// # Date       : 11.03.2013
// 
// # Description:
// # Standalone RGB LED Strip controlled with an IR remote. 
// # Uses electret microphone for music and clap sensing.
// 
// #  This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// #############################################################################

// *********************************************
// INCLUDE
// *********************************************
// # Non-standard libraries:
// # IR Remote Library https://github.com/shirriff/Arduino-IRremote

#include <IRremote.h>
#include "rgbLED.h"

// *********************************************
// PINOUT ASSIGN
// *********************************************
const int irPin = 2;
const int micPin = 3;
const int redPin = 6;
const int greenPin = 5;
const int bluePin = 9;

// *********************************************
// OBJECT DECLARATIONS AND GLOBAL VARIABLES
// *********************************************
const boolean DEBUG = false;
//LED object
rgbLED led(redPin, greenPin, bluePin);
//IR receiver object & variable
IRrecv irrecv(irPin);
decode_results results;
//Effects bit flag
enum Effects {
  Clap = 1,
  MusicOneColour = 2,
  MusicBlink = 4,
  MusicNoBlink = 8,
};
uint8_t effectsFlag = 0;
//Variables used in interrupt functions
volatile boolean micState = false;
volatile uint8_t ledOffTurn = 0;

// *********************************************
// SETUP
// *********************************************
void setup() {
  irrecv.enableIRIn();
  attachInterrupt(1, soundModes, CHANGE);
  //Random used in sound-sensitive modes
  randomSeed(analogRead(0)); 
  if (DEBUG) {
    Serial.begin(115200);
    Serial.println(".::[ SoundLights ]::.");
  }
}

// *********************************************
// LOOP
// *********************************************
void loop() {
   //Reads IR command and calls action
   if (irrecv.decode(&results)) {
		unsigned long hash = decodeHash(&results);
		switch (hash) {
			//TOP 4 BUTTONS
			case 0x40572195: //ON
                                effectsFlag = 0;
				led.setOn();
				if (DEBUG) { Serial.println("On"); }
				break;
				
			case 0xE16B3675: //OFF
                                effectsFlag = 0;
				led.setOff();
				if (DEBUG) { Serial.println("Off"); }
				break;

			case 0xD255EB35: //-BRIGHTNESS	
				led.lessBrightness();
				if (DEBUG) { Serial.println("- Brightness"); }
				break;

			case 0x732CDDD5: //+BRIGHTNESS
				led.moreBrightness();
				if (DEBUG) { Serial.println("- Brightness"); }                                
				break;

			//PRIMARY COLOURS BUTTONS
			case 0xF29018F5: //WHITE
				led.setColour(WHITE);
				if (DEBUG) { Serial.println("White"); }
				break;

			case 0xB8EF1D75: //RED
				led.setColour(RED);
				if (DEBUG) { Serial.println("Red"); }
				break;

			case 0x18182AD5: //GREEN
				led.setColour(GREEN);
				if (DEBUG) { Serial.println("Green"); }
				break;

			case 0x2F323F15: //BLUE

				led.setColour(BLUE);
				if (DEBUG) { Serial.println("Blue"); }
				break;

			//EFFECTS BUTTONS
			case 0x3D3B3035: //FLASH - CLAP MODE
				effectsFlag = Clap;
                                led.setOff();
				led.setOn();
				led.setOff();
				if (DEBUG) { Serial.println("Clap Mode"); }
				break;

			case 0xAD80A955: //STROBE - MUSIC ONE COLOUR
                                effectsFlag = MusicOneColour;
                                led.setOff();
				led.setOn();
				led.setOff();
  				if (DEBUG) { Serial.println("Strobe Mode"); }
				break;

			case 0x363EA735: //FADE - MUSIC BLINK MODE
                                effectsFlag = MusicBlink;
				led.setColour(WHITE);
				led.setOff();  
				if (DEBUG) { Serial.println("Music Blink Mode"); }
				break;

			case 0xE8779E95: //SMOOTH - MUSIC NOBLINK MODE
                                effectsFlag = MusicNoBlink;
                                led.setColour(WHITE);
				led.setOff();
				if (DEBUG) { Serial.println("Music NoBlink Mode"); }
				break;

			//SECONDARY COLOUR BUTTONS
			case 0x7010EC75: //RED 2
				led.setColour(255,50,0);
				break;

			case 0xFDFE8D15: //RED 3
				led.setColour(255,100,0);
				break;

			case 0x69146375: //RED 4
				led.setColour(255,150,0);
				break;
									  
			case 0xE8AFC755: //RED 5
				led.setColour(255,200,0);
				break;

			case 0xCF39F9D5: //GREEN 2
				led.setColour(0,255,50);
				break;

			case 0x5D279A75: //GREEN 3
				led.setColour(0,255,100);
				break;

			case 0xC83D70D5: //GREEN 4
				led.setColour(0,255,150);
				break;

			case 0x47D8D4B5: //GREEN 5
				led.setColour(0,255,200);
				break;

			case 0xDC25A355: //BLUE 2
				led.setColour(25,0,255);
				break;

			case 0x7441AEB5: //BLUE 3
				led.setColour(50,0,255);
				break;

			case 0x24A59D15: //BLUE 4
				led.setColour(100,0,255);
				break;

			case 0x726CA5B5: //BLUE 5
				led.setColour(150,0,255);
				break;
				
			default:  
                                if (DEBUG) { 
    				    Serial.print("Unknown ");
    				    Serial.println(hash, HEX);
                                }
    				break;
		}
		// Resume decoding (necessary!)
		irrecv.resume(); 
	}
}

// *********************************************
// INTERRUPT FUNCTION
// *********************************************
//Called each time mic detects sound. Depending on effect flag will do the assigned mode
void soundModes(){
  //Clap
  if (effectsFlag & Clap) {
      led.setOn();
  }
  //One Colour Music
  if (effectsFlag & MusicOneColour) {
    micState = !micState;
    if (micState) { led.setOn(); }
    else { led.setOff(); } 
  }
  //Music Blink
  if (effectsFlag & MusicBlink) {
    micState = !micState;
    if (micState) { 
      //Will allways have one colour off (to prevent too much whiteness)
      switch (ledOffTurn) {
        case 0: 
              led.setColour(random(0,256),random(0,256),0);
              break;
        case 1: 
              led.setColour(random(0,256),0,random(0,256));
              break;
        case 2: 
              led.setColour(0,random(0,256),random(0,256));
              break;
        default: 
              ledOffTurn = 0;
              break;
      }
      led.setOn();
      ledOffTurn += 1;
      if (ledOffTurn > 2) { ledOffTurn = 0; }
    }
    else { led.setOff(); } 
  }
  //Music No Blink
  if (effectsFlag & MusicNoBlink) {
      //Will allways have one colour off (to prevent too much whiteness)
      switch (ledOffTurn) {
        case 0: 
              led.setColour(random(0,256),random(0,256),0);
              break;
        case 1: 
              led.setColour(random(0,256),0,random(0,256));
              break;
        case 2: 
              led.setColour(0,random(0,256),random(0,256));
              break;
        default: 
              ledOffTurn = 0;
              break;
      }
      led.setOn();
      ledOffTurn += 1;
      if (ledOffTurn > 2) { ledOffTurn = 0; }
  }
}

// *********************************************
// IR REMOTE FUNCTIONS
// *********************************************
// Use FNV hash algorithm: http://isthe.com/chongo/tech/comp/fnv/#FNV-param
#define FNV_PRIME_32 16777619
#define FNV_BASIS_32 2166136261

/* Compare two tick values, returning 0 if newval is shorter,
 * 1 if newval is equal, and 2 if newval is longer
 * Use a tolerance of 20%
 */
int compare(unsigned int oldval, unsigned int newval) {
  if (newval < oldval * 0.8) { return 0; } 
  else if (newval > oldval * 0.8) { return 2; } 
  else { return 1; }
}

/* 
 * Converts the raw code values into a 32-bit hash code.
 * Hopefully this code is unique for each button.
 */
unsigned long decodeHash(decode_results *results) {
  unsigned long hash = FNV_BASIS_32;
  for (int i = 1; i+2 < results->rawlen; i++) {
    int value =  compare(results->rawbuf[i], results->rawbuf[i+2]);
    // Add value into the hash
    hash = (hash * FNV_PRIME_32) ^ value;
  }
  return hash;
}
