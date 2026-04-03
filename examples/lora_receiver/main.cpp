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
    byte receivedData[packetSize];
    bool success = board.readRadioBytes(packetSize, receivedData);
    if (success) {
      Serial.print(F("Data: ["));
      for (int i = 0; i < packetSize; i++) {
        Serial.printf("0x%02X", receivedData[i]);
        if (i < packetSize - 1) Serial.print(F(", "));
      }
      Serial.println(F("]"));
      
      Serial.println(F("Blinking LED to indicate packet received, if supported by the board ..."));
      board.blinkLed();
      delay(400);
      Serial.println(F("Disabling LED after packet received"));
      board.disableLed();
    }
    else {
      Serial.println(F("Failed to read received data"));
    }
  }

  delay(2);
}
