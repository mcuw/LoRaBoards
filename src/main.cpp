#include <Arduino.h>
#include "LgLoraBoard.h"

LgLoraBoard board;

void setup() {
  Serial.begin(115200);
  Serial.println("LoRa Application");

  board.setupRadioBoard();
}

void loop() {
  delay(1000);
}
