#include "Buttons.h"
#include <Arduino.h>

void Buttons::init() {
  pinMode(_selectPin, INPUT_PULLUP);
  pinMode(_upPin, INPUT_PULLUP);
  pinMode(_downPin, INPUT_PULLUP);
  _previousTime = millis();
}

void Buttons::update() {
  unsigned long currentTime = millis();
  if (currentTime - _previousTime < 20) return;
  _previousTime = currentTime;
  _selectVal = digitalRead(_selectPin);
  _downVal = digitalRead(_downPin);
  _upVal = digitalRead(_upPin);
}