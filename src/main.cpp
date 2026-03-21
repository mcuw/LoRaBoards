#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <SPI.h>
#include <SD.h>
#include <RadioLib.h> // https://jgromes.github.io/RadioLib/class_s_x1262.html
// https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/blob/master/examples/TBeamFactory/TBeamFactory.ino
#include "LoRaBoards.h"

// user configuration
#if defined(USING_SX1262)
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           868.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   22
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             125.0
#endif
#endif

SX1262 radio = new Module(LORA_CS, LORA_IRQ, LORA_RST, LORA_IO2); // NSS, RST, DIO0
// counter to keep track of transmitted packets
int count = 0;

void setup() {
  Serial.begin(115200);

  Serial.println("LoRa Sender");
  SPI.begin(SCK, MISO, MOSI, SS);
  int state = radio.begin();
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("LoRa initialization failed, code: ");
    Serial.println(state);
    delay(5000);
    ESP.restart();
  }
  Serial.println("LoRa initialized successfully!");

  if (radio.setFrequency(CONFIG_RADIO_FREQ) == RADIOLIB_ERR_INVALID_FREQUENCY) {
    Serial.println(F("Selected frequency is invalid for this module!"));
    delay(5000);
    ESP.restart();
  }

  // meshcore 62.5 kHz
  if (radio.setBandwidth(250.0) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
    Serial.println(F("Selected bandwidth is invalid for this module!"));
    delay(5000);
    ESP.restart();
  }

  // set spreading factor to 8
  if (radio.setSpreadingFactor(8) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
    Serial.println(F("Selected spreading factor is invalid for this module!"));
    delay(5000);
    ESP.restart();
  }

  // set coding rate to 8
  if (radio.setCodingRate(8) == RADIOLIB_ERR_INVALID_CODING_RATE) {
    Serial.println(F("Selected coding rate is invalid for this module!"));
    delay(5000);
    ESP.restart();
  }

  // Secure set LoRa sync word to 0xBE
  if (radio.setSyncWord(0xBE) != RADIOLIB_ERR_NONE) {
    Serial.println(F("Unable to set sync word!"));
    delay(5000);
    ESP.restart();
  }

  // // set output power to 10 dBm (accepted range is -17 - 22 dBm)
  // if (radio.setOutputPower(10) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
  //   Serial.println(F("Selected output power is invalid for this module!"));
  //   while (true) { delay(10); }
  // }

  // // set over current protection limit to 80 mA (accepted range is 45 - 240 mA)
  // // NOTE: set value to 0 to disable overcurrent protection
  // if (radio.setCurrentLimit(80) == RADIOLIB_ERR_INVALID_CURRENT_LIMIT) {
  //   Serial.println(F("Selected current limit is invalid for this module!"));
  //   while (true) { delay(10); }
  // }

  // // set LoRa preamble length to 15 symbols (accepted range is 0 - 65535)
  // if (radio.setPreambleLength(15) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
  //   Serial.println(F("Selected preamble length is invalid for this module!"));
  //   while (true) { delay(10); }
  // }

  // // disable CRC
  // if (radio.setCRC(false) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION) {
  //   Serial.println(F("Selected CRC is invalid for this module!"));
  //   while (true) { delay(10); }
  // }

  // // Some SX126x modules have TCXO (temperature compensated crystal
  // // oscillator). To configure TCXO reference voltage,
  // // the following method can be used.
  // if (radio.setTCXO(2.4) == RADIOLIB_ERR_INVALID_TCXO_VOLTAGE) {
  //   Serial.println(F("Selected TCXO voltage is invalid for this module!"));
  //   while (true) { delay(10); }
  // }

  // // Some SX126x modules use DIO2 as RF switch. To enable
  // // this feature, the following method can be used.
  // // NOTE: As long as DIO2 is configured to control RF switch,
  // //       it can't be used as interrupt pin!
  // if (radio.setDio2AsRfSwitch() != RADIOLIB_ERR_NONE) {
  //   Serial.println(F("Failed to set DIO2 as RF switch!"));
  //   while (true) { delay(10); }
  // }
}

void loop() {
    // you can transmit C-string or Arduino string up to
  // 256 characters long
  String str = "Hello World! #" + String(count++);
  Serial.print(F("[SX1262] Transmitting packet ... "));
  int state = radio.transmit(str);

  // you can also transmit byte array up to 256 bytes long
  /*
    byte byteArr[] = {0x01, 0x23, 0x45, 0x56, 0x78, 0xAB, 0xCD, 0xEF};
    int state = radio.transmit(byteArr, 8);
  */

  if (state == RADIOLIB_ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(F("success!"));

  } else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(F("too long!"));

  } else if (state == RADIOLIB_ERR_TX_TIMEOUT) {
    // timeout occured while transmitting packet
    Serial.println(F("timeout!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);

  }

  delay(2000);
}
