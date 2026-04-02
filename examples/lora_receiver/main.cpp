#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <SPI.h>
#include <SD.h>

#include "LgLoraBoard.h"

LgLoraBoard board;

void setup() {
  Serial.begin(115200);
  Serial.println("LoRa Receiver");

  board.setupRadioBoard();
}

void loop() {
  int packetSize = board.hasRadioPacket();
  if (packetSize) {
    byte data[packetSize];
    board.readRadioBytes(packetSize, data);

    Serial.print(F("Data: ["));
    for (int i = 0; i < packetSize; i++) {
      Serial.printf("0x%02X", data[i]);
      if (i < packetSize - 1) Serial.print(", ");
    }
    Serial.println(F("]"));

    Serial.println("Blinking LED to indicate packet received");
    board.blinkLed();
    delay(400);
    Serial.println("Disabling LED after packet received");
    board.disableLed();
  }

  delay(2);
}
