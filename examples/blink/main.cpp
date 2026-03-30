#include <Arduino.h>
#include "LgLoraBoard.h"

LgLoraBoard board;

void setup() {
  Serial.begin(115200);
  Serial.println(F("Blink example"));

  board.setupRadioBoard();
}

void loop() {
  board.flashLed(50);
  delay(2);
}
