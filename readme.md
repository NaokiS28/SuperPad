# Super Pad
Basic library for using NES and Super NES pads on an arduino with auto detection between the two.

Basic usage:
```
#include "SuperPad.h"

const int Data = 2;
const int Data = 3;
const int Data = 4;

int controllerData = 0;     //  x,x,x,x, TR, TL, X, Y, SEL, STRT, B, A, Ri, Lf, Dn, Up

SuperPad pad(Data, Clock, Latch);     // HardwareSerial port, Sense pin

void setup(){
    Serial.begin(9600);

    // Print pad type
    Serial.print(F("Controller connected: "));
    if(pad.type() == 1){
        Serial.println(F("SNES"));
    } else {
        Serial.println(F("NES"));
    }
}

void loop(){
    if(pad.update()){
        controllerData = pad.read();
        Serial.print("Pad: ");
        Serial.println(controllerData, BIN);
    }
}

```

