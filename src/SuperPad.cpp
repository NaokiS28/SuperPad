#ifndef SUPERPAD_CPP
#define SUPERPAD_CPP

/*
 *   SuperPAD - CPP
 *   ==============
 *   NKS ©2021
 *      
 *   Reads either Nintendo NES or Super NES controllers with auto detection between the two.
 * 
*/

#include "superpad.h"

SuperPad::SuperPad(int d, int c, int l, int t){
    data = d;
    clock = c;
    latch = l;
    padType = t;
    pinMode(data, INPUT_PULLUP);
    pinMode(clock, OUTPUT);
    pinMode(latch, OUTPUT);
    digitalWrite(latch, LOW);
    digitalWrite(clock, LOW);
    if(padType != 255){
        lockType = true;
    }
};

bool SuperPad::update(){
    buttons = read();
    if(buttons != lastButtons && !interrupt){
        lastButtons = buttons;
        interrupt = true;
    }
    return interrupt;
}      // Returns true if buttons have changed

void SuperPad::type(uint8_t t){
    padType = t;
    lockType = true;
}    // Force pad type. Will always use this type of pad. 
    // Warning, this will disable the class if it's above 1

uint16_t SuperPad::read(){
    uint16_t bIn = 0;

    latchIn();

    uint8_t b = shiftIn(data, clock, MSBFIRST);
    bIn |= ((b & 0xF0) >> 4);   // Map direction buttons
    bIn |= ((b & 0x0F) << 4);   // Move face buttons after directions

    if(padType == 1){
        // SNES
        b = shiftIn(data, clock, MSBFIRST);
        bIn |= ((b & 0xF0) << 4);    // Hi Byte (discard the ID bits)
        // We swap A and Y to keep consistant with programs
        bool a = bitRead(bIn, 6);
        bool y = bitRead(bIn, 15);
        bitWrite(bIn, 6, y);
        bitWrite(bIn, 15, a);
    }
    interrupt = false;
    return bIn;
}    // Returns byte if NES and int if SNES

uint8_t SuperPad::detectPad(){
    latchIn();
    shiftIn(data, clock, MSBFIRST);
    uint8_t b = shiftIn(data, clock, MSBFIRST);
    // SNES has bits 8-11 high, NES grounds Serial in
    if((b & 0x0F) == 0x00) {
        // NES
        padType = 0;
    } else {
        // SNES
        padType = 1;
    }
}
uint8_t SuperPad::readByte(){
    return (read() & 0xFF);
}         // Read first byte (and only byte on NES)

uint16_t SuperPad::readData(){
    return read();
}        // Returns whole int of buttons

void SuperPad::latchIn(){
    digitalWrite(clock, LOW);   // CD4021 is active hi, shiftIn works backwards so this keeps our first bit
    digitalWrite(latch, HIGH);
    delayMicroseconds(1);       // This is here to allow the CD4021's sufficient time to load
    digitalWrite(latch, LOW);
}

#endif