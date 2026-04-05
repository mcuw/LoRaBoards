#include <Arduino.h>
#include <LgLoraBoard.h>

LgLoraBoard board;

void setup() {
  Serial.begin(115200);
  Serial.println(F("LED blink example"));

  board.setupRadioBoard();
  board.blinkLed(100);
}

void loop() {
  vTaskDelete(NULL); // current task is not needed anymore or delete this line to keep the loop running

  // Serial.println(F("Stop blinking."));
  // board.disableLed();
  // delay(5000);

  // Serial.println(F("Blinking..."));
  // board.blinkLed(100);
  // delay(5000);
}
