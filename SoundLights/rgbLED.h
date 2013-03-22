// #############################################################################
// 
// # Name       : rgbLed
// # Version    : 1.5

// # Author     : Juan L. Perez Diez <ender.vs.melkor at gmail>
// # Date       : 11.03.2013
// 
// # Description: Library for controlling an RGB LED, 
// # Allows to set colour, change brightness and do some effects.
// # Constructor needs red, green and blue pins as argument.
// # Keep in mind delays are used in effects functions.
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

#ifndef __RGBLED_H
#define __RGBLED_H

#include <Arduino.h>

enum color { RED, GREEN, BLUE, WHITE };

class rgbLED {
  public:
     rgbLED(uint8_t, uint8_t, uint8_t);
     void setOn();
     void setOff();
     void setColour(color);
     void setColour(uint8_t, uint8_t, uint8_t);
     void moreBrightness();
     void lessBrightness();
     //Effects - They use delays!
     void strobe();
     void rgbStrobe();
     void smooth();
     //void fade();

  private:
     uint8_t _redPin;
     uint8_t _greenPin;
     uint8_t _bluePin;
     uint8_t _red;
     uint8_t _green;
     uint8_t _blue;
     //long _fadeTime;
};

//Constructor
rgbLED::rgbLED(uint8_t r, uint8_t g, uint8_t b) : 
      _redPin(r), _greenPin(g), _bluePin(b) { 
    pinMode(_redPin, OUTPUT);
    pinMode(_greenPin, OUTPUT);
    pinMode(_bluePin, OUTPUT);
    _red = 255;
    _green = 255;
    _blue = 255;
}

void rgbLED::setOn() {
    analogWrite(_redPin,_red);
    analogWrite(_greenPin,_green);
    analogWrite(_bluePin,_blue);
}

void rgbLED::setOff() {
    digitalWrite(_redPin, LOW);
    digitalWrite(_greenPin, LOW);
    digitalWrite(_bluePin, LOW);
}

//Drives led coloring
void rgbLED::setColour(color c) {
   switch(c) {
      case RED:    
        _red = 255;
        _green = 0;
        _blue = 0;
        break;
      case GREEN:   
        _red = 0;
        _green = 255;
        _blue = 0;
        break;
      case BLUE:  
        _red = 0;
        _green = 0;
        _blue = 255;
        break;
      case WHITE:  
        _red = 255;
        _green = 255;
        _blue = 255;
        break;
      default: break;
   } 
   setOn();
}

//Colors the strip (each color [0..255])
void rgbLED::setColour(uint8_t red, uint8_t green, uint8_t blue) {
    _red = red;
    _green = green;
    _blue = blue;
    setOn();
}

//Steps brightness up
void rgbLED::moreBrightness() {
    const int inc = 25; 
    uint8_t red = _red;
    uint8_t green = _green;
    uint8_t blue = _blue;
    //only colors that are on are modified	  	
    if (red > 0) { 
      red += inc; 
      //do not overflow into lowest brightness
      if (_red > red) { red = 255; }   
    }
    if (green > 0) { 
      green += inc; 
      if (_green > green) { green = 255; }
    }
    if (blue > 0) { 
      blue += inc; 
      if (_blue > blue) { blue = 255; }
    }
    setColour(red,green,blue);
}

//Steps brightness down
void rgbLED::lessBrightness() {
    const int inc = 25; 
    uint8_t red = _red;
    uint8_t green = _green;
    uint8_t blue = _blue;
    //only colors that are on are modified	  	
    if (red > 0) { 
      red -= inc; 
      //keep color into bounds:
      if (_red < red) { red = 5; }   
    }
    if (green > 0) { 
      green -= inc; 
      if (_green < green) { green = 5; }
    }
    if (blue > 0) { 
      blue -= inc; 
      if (_blue < blue) { blue = 5; }
    }
    setColour(red,green,blue);
}

//Turns led on-off
void rgbLED::strobe() {
    setOff();
    delay(500);
    setOn();
    delay(250);
}
  
//Cycles rapidly through full Red-Green-Blue
void rgbLED::rgbStrobe() {
    setColour(RED);
    delay(250);
    setColour(GREEN);
    delay(250);
    setColour(BLUE);
    delay(250);
}

//Cycles through every colour possible.
//Heavy use of delays so program will become unresponsive for a long time!
void rgbLED::smooth() {
  //Reduce to make faster
  const int FADESPEED = 5;
  uint8_t r,g,b;
  // fade from blue to violet
  for (r = 0; r < 256; r++) { 
    analogWrite(_redPin, r);
    delay(FADESPEED);
  } 
  // fade from violet to red
  for (b = 255; b > 0; b--) { 
    analogWrite(_bluePin, b);
    delay(FADESPEED);
  } 
  // fade from red to yellow
  for (g = 0; g < 256; g++) { 
    analogWrite(_greenPin, g);
    delay(FADESPEED);
  } 
  // fade from yellow to green
  for (r = 255; r > 0; r--) { 
    analogWrite(_redPin, r);
    delay(FADESPEED);
  } 
  // fade from green to teal
  for (b = 0; b < 256; b++) { 
    analogWrite(_bluePin, b);
    delay(FADESPEED);
  } 
  // fade from teal to blue
  for (g = 255; g > 0; g--) { 
    analogWrite(_greenPin, g);
    delay(FADESPEED);
  } 
}
#endif
