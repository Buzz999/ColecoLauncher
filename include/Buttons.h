#pragma once

#include <stdint.h>

class Buttons {
 public:
  Buttons(uint8_t selectPin, uint8_t upPin, uint8_t downPin)
      : _selectPin(selectPin), _upPin(upPin), _downPin(downPin) {}

  void init();
  void update();

  bool selectPressed() const { return not _selectVal; }
  bool upPressed() const { return not _upVal; }
  bool downPressed() const { return not _downVal; }
  void waitUnPressed() { 
    while (1) {
      update();
      if ((not selectPressed()) && (not upPressed()) && (not downPressed())) return;
    }
  }

 protected:
  uint8_t _selectPin, _upPin, _downPin;
  bool _selectVal, _upVal, _downVal;
  unsigned long _previousTime;
};
