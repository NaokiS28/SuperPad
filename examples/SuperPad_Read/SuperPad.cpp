#ifndef SUPERPAD_CPP
#define SUPERPAD_CPP

/*
     SuperPAD - CPP
     ==============
     NKS ©2021

     Reads either Nintendo NES or Super NES controllers with auto detection between the two.

*/

#include "SuperPad.h"

SuperPad::SuperPad(int d, int c, int l, int t) {
  data = d;
  clock = c;
  latch = l;
  padType = t;
  pinMode(data, INPUT_PULLUP);
  pinMode(clock, OUTPUT);
  pinMode(latch, OUTPUT);
  digitalWrite(latch, LOW);
  digitalWrite(clock, HIGH);
  if (padType != 255) {
    lockType = true;
  }
}

bool SuperPad::update() {
  uint16_t in = read();
  if (in != lastButtons && !interrupt) {
    lastButtons = in;
    interrupt = true;
  }
  return interrupt;
}      // Returns true if buttons have changed

void SuperPad::type(uint8_t t) {
  padType = t;
  lockType = true;
}    // Force pad type. Will always use this type of pad.
// Warning, this will disable the class if it's above 1

uint16_t SuperPad::read() {
  uint16_t bIn = 0x0000;

  latchIn();

  uint8_t b = ~serialIn(data, clock);

  bIn |= ((b & 0xF0) >> 4);   // Map direction buttons
  bIn |= ((b & 0x0F) << 4);   // Move face buttons after directions


  if (padType == 1) {
    // SNES
    b = ~serialIn(data, clock);
    bIn |= ((b & 0xF0) << 4);    // Hi Byte (discard the ID bits)
    // We swap A and Y to keep consistant with programs
    bool a = bitRead(bIn, 6);
    bool y = bitRead(bIn, 15);
    bitWrite(bIn, 6, y);
    bitWrite(bIn, 15, a);
  }
  buttons = bIn;
  interrupt = false;
  return bIn;
}    // Returns byte if NES and int if SNES

uint8_t SuperPad::detectPad() {
  latchIn();
  serialIn(data, clock);
  uint8_t b = serialIn(data, clock);
  // SNES has bits 8-11 high, NES grounds Serial in
  if ((b & 0x0F) == 0x00) {
    // NES
    padType = 0;
  } else {
    // SNES
    padType = 1;
  }
  return padType;
}
uint8_t SuperPad::readByte() {
  return (read() & 0xFF);
}         // Read first byte (and only byte on NES)

uint16_t SuperPad::readData() {
  return read();
}        // Returns whole int of buttons

void SuperPad::latchIn() {
  digitalWrite(clock, HIGH); 
  digitalWrite(latch, HIGH);
  delayMicroseconds(1);       // This is here to allow the CD4021's sufficient time to load
  digitalWrite(latch, LOW);
}

uint8_t SuperPad::type() {
  detectPad();
  return padType;
}

uint8_t SuperPad::serialIn(uint8_t dataPin, uint8_t clockPin) {
  uint8_t value = 0;
  uint8_t i;
  for (i = 0; i < 8; ++i) {
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(2);   // Teensy is way too fast
    value |= (digitalRead(dataPin) << i);
    digitalWrite(clockPin, LOW);
    delayMicroseconds(2);
  }
  return value;
}

#endif
