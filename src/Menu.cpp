#include "Menu.h"

bool Menu::prev() {
    if (_iSel > _iWin) {
      _iSel--;
      return true;
    }

    if (_iSel > 0) {
      _iSel--;
      _iWin--;
      return true;
    }
    return false;
  }

 bool Menu::next() {
    if (_n == 0) return false;
    if (_iSel < _n-1) {
      if (_iSel >= _iWin + _nLin - 1) {
               _iWin++;
      }
      _iSel++;
      return true;
    }
    return false;
  }