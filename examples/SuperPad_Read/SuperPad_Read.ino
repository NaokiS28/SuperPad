/*
   SuperPad Read demo
   NKS 2021
   =================
   
*/

#include "SuperPad.h"

const byte DAT = 38;
const byte CLK = 39;
const byte LAT = 37;

// Initializer
SuperPad pad(DAT, CLK, LAT);

uint16_t lastButtons = 0;
uint16_t padButtons = 0;

// This holds the last poll time. This is used as reading quickly can lead to multiple triggers.
uint32_t padDebounce = 0;
uint32_t padTimer = 0;
uint8_t padType;

String buttons[3][12] = {
  {"Up", "Down", "Left", "Right", "", "1", "2", "", "", "", "", ""},
  {"Up", "Down", "Left", "Right", "A", "B", "C", "Start", "", "", "", ""},
  {"Up", "Down", "Left", "Right", "A", "B", "C", "Start", "X", "Y", "Z", "Mode"}
};

void setup() {
  Serial.begin(9600);
  Serial.println("SuperPad Read Demo");

  //padType is an enum that has the states of SMS, MD3 or MD6, for MasterSystem, 3 button and 6 putton types.
  // If select pin is not given, or no pad is connected, the library will default to SMS mode
  padType = pad.type();

  Serial.print("Controller type: ");
  switch (padType) {
    case 0:
      Serial.println("NES");
      break;

    case 1:
      Serial.println("SNES");
      break;
  }
}

void loop() {
  // Get new buttons
  padButtons = pad.read();

  // Read the new button states if there are any
  if (padButtons != lastButtons && (millis() - padDebounce >= 10)) {
    // Store the time to stop bouncing
    padDebounce = millis();

    // This prints the binary representation of the controller buttons
    Serial.print('B');
    Serial.print(padButtons, BIN);
    Serial.print(" - ");
    // This prints the names of the buttons pressed
    bool buttonPressed = false;
    /*
    for (int p = 0; p < 12; p++) {
      if (bitRead(padButtons, p) == true) {
        if (buttonPressed == true) {
          Serial.print(", ");
        }
        Serial.print(buttons[padType][p]);
        buttonPressed = true;
      }
    }*/
    Serial.println();

    // Store the current button states to compare
    lastButtons = padButtons;
  }

  // Auto detect a pad being changed. This would be better to do as a timed function in real world uses
  //  to keep the overhead down but for this example this will work fine
  // We need to do this when the buttons aren't pressed or you will get glitched results.
  //  this is because we know that if the pad is removed, then no buttons are pressed.
  if (millis() - padTimer >= 250 && padButtons == 0) {
    padTimer = millis();
    byte test = pad.type();
    if (padType != test) {
      padType = test;
      Serial.print("Controller type: ");
      switch (padType) {
        case 0:
          Serial.println("NES");
          break;
    
        case 1:
          Serial.println("SNES");
          break;
      }
    }
  }
}
