#include <Arduino.h>
#include <RadioLib.h> // https://jgromes.github.io/RadioLib/class_s_x1262.html
#include <LgLoraBoard.h>

LgLoraBoard board;
volatile bool buttonClicked = false;

void onClickInterrupt() {
  buttonClicked = true;
}

void setup() {
  Serial.begin(115200);

  Serial.println("LoRa Sender");

  board.setupRadioBoard();
  board.printBoardStatus();

  board.setupButton(onClickInterrupt);
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
