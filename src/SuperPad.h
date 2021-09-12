#ifndef SUPERPAD_H
#define SUPERPAD_H

/*
 *  SuperPAD
 *  ========
 *  NKS ©2021
 * 
 *  Mapping is as follows, from MSB to LSB
 *  15                                                   0
 *  x,x,x,x, TR, TL, X, Y, SEL, STRT, B, A, Ri, Lf, Dn, Up
 * 
*/

#include <Arduino.h>

class SuperPad {
    public:
        SuperPad(int d, int c, int l, int t = 255);
        bool update();              // Returns true if buttons have changed
        void    type(uint8_t t);    // Force pad type. Will always use this
        uint8_t type();             // Return type ID (0: NES, 1: SNES)
        uint16_t read();            // Returns byte if NES and int if SNES

    private:
        // Pins
        bool interrupt = 0;
        bool lockType = 0;
        uint16_t data, clock, latch, buttons, lastButtons;
        uint8_t padType;            // 0 = NES 4D2B pad, 1 = SNES 4D 6B pad

        void latchIn();
        uint8_t detectPad();
        uint8_t readByte();         // Read first byte (and only byte on NES)
        uint8_t readByte(bool r);   // Read byte stored at register 0 or 1
        uint16_t readData();        // Returns whole int of buttons
        uint8_t serialIn(uint8_t dataPin, uint8_t clockPin); 

};

#endif
