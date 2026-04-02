#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <SPI.h>
#include <SD.h>
#include <RadioLib.h> // https://jgromes.github.io/RadioLib/class_s_x1262.html
// https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/blob/master/examples/TBeamFactory/TBeamFactory.ino
#include "LgLoraBoard.h"

// SX1262 radio = new Module(LORA_CS, LORA_IRQ, LORA_RST, LORA_IO2); // NSS, RST, DIO0
// counter to keep track of transmitted packets
int count = 0;
LgLoraBoard board;
volatile bool buttonClicked = false;

void onButtonClick() {
  buttonClicked = true;
}

void onButtonDoubleClick() {
}

void onButtonLongPress() {
}

void setup() {
  Serial.begin(115200);

  Serial.println("LoRa Sender");

  board.setupRadioBoard();
  board.printBoardStatus();

  board.setupButton(onButtonClick, onButtonDoubleClick, onButtonLongPress);
}

void loop() {
  if (buttonClicked) {
    buttonClicked = false;
  
    byte byteArr[] = {0x01, 0x23, 0x45, 0x56, 0x78, 0xAB, 0xCD, 0xEF};
    int state = board.transmitRadioBytes(8, byteArr);


    switch (state) {
      case RADIOLIB_ERR_UNKNOWN:
        Serial.println(F("An unknown error occurred during packet transmission"));
        break;

      case RADIOLIB_ERR_CHIP_NOT_FOUND:
        Serial.println(F("LoRa radio not found"));
        break;

      // the packet was successfully transmitted
      case RADIOLIB_ERR_NONE:
        Serial.println(F("The packet was successfully transmitted"));
        break;

      // the supplied packet was longer than maximum allowed package size (255 bytes)
      case RADIOLIB_ERR_PACKET_TOO_LONG:
        Serial.println(F("The packet is too long to be transmitted"));
        break;

      // timeout occured while transmitting packet
      case RADIOLIB_ERR_TX_TIMEOUT:
        Serial.println(F("The packet transmission timed out"));
        break;

      default:
        Serial.print(F("The packet transmission failed, code "));
        Serial.println(state);
        break;
    }
  }

  delay(2);
}
