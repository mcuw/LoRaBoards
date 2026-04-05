#include <Arduino.h>
#include <LgLoraBoard.h>

LgLoraBoard board;

void onButtonClick() {
  Serial.println("Button clicked");
}

void onButtonDoubleClick() {
  Serial.println("Button double clicked");
}

void onButtonLongPress() {
  Serial.println("Button long pressed");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Button example");

  board.setupButton(onButtonClick, onButtonDoubleClick, onButtonLongPress);
}

void loop() {
  vTaskDelete(NULL); // current task is not needed anymore or delete this line to keep the loop running

  // delay(2);
}
