#pragma once

#include <stdint.h>

class Menu {

public:
  Menu() : _nLin(16) { reset(); }

  void reset() {
    _n = 0;
    _iSel = 0;
    _iWin = 0;
  }

  bool prev();
  bool next();
  void setNEntries(uint8_t n) { _n = n;  }

  uint8_t getISel() const { return _iSel; }
  uint8_t getIWin() const { return _iWin; }
  uint8_t getNDraw() { return  (_n < _nLin) ? _n : _nLin; }

protected:
  uint8_t _nLin, _n;
  uint8_t _iSel, _iWin;
};