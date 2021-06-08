#include <Arduino.h>
#include "Control.h"

Control ctrl;

void setup() { ctrl.init(); }
void loop() { ctrl.update(); }