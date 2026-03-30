#include <SPI.h>
#include <WiFi.h>
#include <RadioLib.h> // https://jgromes.github.io/RadioLib
#include "LgLoraBoard.h"
#include "devices/LgButton.h"
#include "devices/LgLed.h"
// #include "devices/LgBluetooth.h"
// #include "devices/LgDisplay.h"
// #include "devices/LgGps.h"
// #include "devices/LgPmu.h"
// #include "devices/LgNtc.h"
// #include "devices/LgSdCard.h"
// #include "devices/LgTwoWire.h"

#include "soc/rtc.h"
#ifdef ENABLE_BLE
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#endif

#if defined(HAS_SDCARD) && !defined(SD_SHARE_SPI_BUS)
SPIClass SDCardSPI(HSPI);
#endif

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#include "hal/gpio_hal.h"
#endif
#include "driver/gpio.h"

uint32_t LgLoraBoard::deviceOnline = 0x00;

static const char *TAG = "LgLoraBoard";
static DevInfo_t devInfo;

#if defined(USE_SX1262) || defined(USE_SX1276)
// flag to indicate that a packet was received
volatile bool receivedFlag = false;
#if defined(ESP32)
ICACHE_RAM_ATTR
#endif
// we got a packet, set the flag
void setFlag()
{
  receivedFlag = true;
}
// String str;

#ifdef USE_SX1262
SX1262 radio = new Module(LORA_CS, LORA_IRQ, LORA_RST, LORA_IO2); // NSS, RST, DIO0
#endif                                                            // USE_SX1262

#ifdef USE_SX1276
SX1276 radio = new Module(LORA_CS, LORA_IRQ, LORA_RST); // NSS, RST, DIO0
#endif                                                  // USE_SX1276
#endif                                                  // USE_SX1262 || USE_SX1276


LgLoraBoard::LgLoraBoard() :
#ifdef HAS_BUTTON
  button(new LgButton()),
#endif
#ifdef HAS_LED
  led(new LgLed())
#endif
{
};

LgLoraBoard::~LgLoraBoard()
{
#ifdef HAS_BUTTON
  if (button) {
    delete button;
    button = nullptr;
  }
#endif
#ifdef HAS_LED
  if (led) {
    delete led;
    led = nullptr;
  }
#endif
}

void LgLoraBoard::printWakeupReason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_UNDEFINED:
    ESP_LOGD(TAG, "Reset reason: In case of deep sleep, reset was not caused by exit from deep sleep");
    break;
  case ESP_SLEEP_WAKEUP_ALL:
    break;
  case ESP_SLEEP_WAKEUP_EXT0:
    ESP_LOGD(TAG, "Reset reason: Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    ESP_LOGD(TAG, "Reset reason: Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    ESP_LOGD(TAG, "Reset reason: Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    ESP_LOGD(TAG, "Reset reason: Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    ESP_LOGD(TAG, "Reset reason: Wakeup caused by ULP program");
    break;
  default:
    ESP_LOGD(TAG, "Reset reason: Wakeup was not caused by deep sleep: %d", wakeup_reason);
    break;
  }
}

void LgLoraBoard::printChipInfo()
{
  ESP_LOGD(TAG, "-----------------------------------");

  printWakeupReason();

  if (psramFound())
  {
    uint32_t psram = ESP.getPsramSize();
    devInfo.psramSize = psram / 1024.0 / 1024.0;
    ESP_LOGD(TAG, "PSRAM is enable! PSRAM: %.2fMB", devInfo.psramSize);
    deviceOnline |= PSRAM_ONLINE;
  }
  else
  {
    ESP_LOGD(TAG, "PSRAM is disable!");
    devInfo.psramSize = 0;
  }

  devInfo.flashSize = ESP.getFlashChipSize() / 1024.0 / 1024.0;
  devInfo.flashSpeed = ESP.getFlashChipSpeed() / 1000 / 1000;
  devInfo.chipModel = ESP.getChipModel();
  devInfo.chipModelRev = ESP.getChipRevision();
  devInfo.chipFreq = ESP.getCpuFreqMHz();

  ESP_LOGD(TAG, "Flash Size: %d MB", devInfo.flashSize);
  ESP_LOGD(TAG, "Flash Speed: %d Mb/s", devInfo.flashSpeed);
  ESP_LOGD(TAG, "Model: %s", devInfo.chipModel.c_str());

  ESP_LOGD(TAG, "Chip Revision: %d", devInfo.chipModelRev);
  ESP_LOGD(TAG, "Freq: %d MHZ", devInfo.chipFreq);
  ESP_LOGD(TAG, "SDK Ver: %s", ESP.getSdkVersion());
  ESP_LOGD(TAG, "DATE: %s", __DATE__);
  ESP_LOGD(TAG, "TIME: %s", __TIME__);

  uint8_t mac[6];
  char macStr[18] = {0};
  esp_efuse_mac_get_default(mac);
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  ESP_LOGD(TAG, "EFUSE MAC: %s", macStr);

  ESP_LOGD(TAG, "-----------------------------------");
}

void LgLoraBoard::setupRadioBoard()
{
  ESP_LOGD(TAG, "setupRadioBoard called");

  printChipInfo();

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

#if defined(HAS_SDCARD)
#if defined(SD_SHARE_SPI_BUS)
  // Share spi bus with lora , set lora cs,rst to high
  pinMode(LORA_CS, OUTPUT);
  pinMode(LORA_RST, OUTPUT);
  digitalWrite(LORA_CS, HIGH);
  digitalWrite(LORA_RST, HIGH);
#else
  SDCardSPI.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_CS);
#endif /*SD_SHARE_SPI_BUS*/
#endif // HAS_SDCARD

#ifdef I2C1_SDA
  Wire1.begin(I2C1_SDA, I2C1_SCL);
#endif

#ifdef HAS_GPS
#ifdef GPS_EN_PIN
  pinMode(GPS_EN_PIN, OUTPUT);
  digitalWrite(GPS_EN_PIN, HIGH);
#endif // GPS_EN_PIN

#ifdef GPS_PPS_PIN
  pinMode(GPS_PPS_PIN, INPUT);
#endif

  SerialGPS.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
#endif // HAS_GPS

#if OLED_RST
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, HIGH);
  delay(20);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  delay(20);
#endif

#ifdef HAS_LED
  // T-Beam LED defaults to low level as turn on,
  // so it needs to be forced to pull up
#if LED_ON == LOW
  gpio_hold_dis((gpio_num_t)LED_BUILTIN);
#endif // LED_ON == LOW
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_ON);
#endif

#ifdef GPS_RST_PIN
  pinMode(GPS_RST_PIN, OUTPUT);
  digitalWrite(GPS_RST_PIN, HIGH);
#endif

#ifdef RADIO_LDO_EN
  /*
   * 2W and BPF LoRa LDO enable , Control SX1262 , LNA
   * 2W and BPF  Radio version must set LDO_EN to HIGH to initialize the Radio
   * */
  pinMode(RADIO_LDO_EN, OUTPUT);
  digitalWrite(RADIO_LDO_EN, HIGH);
#endif

#ifdef RADIO_CTRL
  /*
   * 2W and BPF LoRa RX TX Control
   * CTRL controls the LNA, not the PA.
   * Only when RX DATA is on, set to 1 to turn on LNA
   * When TX DATA is on, CTL is set to 0 and LNA is turned off.
   * */
  pinMode(RADIO_CTRL, OUTPUT);
  digitalWrite(RADIO_CTRL, LOW);
#endif

#ifdef RADIO_DIO2_PIN
  pinMode(RADIO_DIO2_PIN, INPUT);
#endif

#ifdef HAS_PMU
  beginPower();
#endif

// Perform an I2C scan after power-on operation
#ifdef I2C_SDA
  Wire.begin(I2C_SDA, I2C_SCL);
  ESP_LOGD(TAG, "==================Scan Wire ==================");
  scanDevices(&Wire);
#endif

#ifdef I2C1_SDA
  ESP_LOGD(TAG, "==================Scan Wire1==================");
  scanDevices(&Wire1);
#endif

  beginSDCard();

#ifdef HAS_DISPLAY
  beginDisplay();
#endif

  // scanWiFi();
  // beginWiFi();

#ifdef FAN_CTRL
  pinMode(FAN_CTRL, OUTPUT);
#endif

// TODO: add GPS recovery function, try to restore factory settings if GPS is not found at startup
#ifdef HAS_GPS
#if defined(T_BEAM_S3_SUPREME) || defined(T_BEAM_1W) || defined(T_BEAM_S3_BPF)
  // T-Beam v1.2 skips L76K
  find_gps = beginGPS();
#endif
  uint32_t baudrate[] = { 9600 }; // 19200, 38400, 57600, 115200, 230400, 460800, 921600, 4800
  if (!find_gps)
  {
    // Restore factory settings
    for (int i = 0; i < sizeof(baudrate) / sizeof(baudrate[0]); ++i)
    {
      ESP_LOGD(TAG, "GPS: Update baudrate : %u", baudrate[i]);
      SerialGPS.updateBaudRate(baudrate[i]);
      if (recoveryGPS())
      {
        ESP_LOGD(TAG, "UBlox GNSS init succeeded, using baudrate: %u", baudrate[i]);
        gps_model = "UBlox";
        find_gps = true;
        break;
      }
    }
  }
  else
  {
    gps_model = "L76K";
  }

  if (find_gps)
  {
    deviceOnline |= GPS_ONLINE;
  }

#ifdef T_BEAM_S3_SUPREME
  enable_slow_clock();
#endif

  bool radio_online = setupRadio(CONFIG_RADIO_RESTART_ON_FAIL);
  if (radio_online)
  {
    deviceOnline |= RADIO_ONLINE;
  }
#endif
}

void LgLoraBoard::printBoardStatus()
{
  bool radioOnline = (deviceOnline & RADIO_ONLINE) != 0;
  ESP_LOGD(TAG, "Radio        : %s", radioOnline ? "+" : "-");

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S3)

  ESP_LOGD(TAG, "PSRAM        : %s", psramFound() ? "+" : "-");

#ifdef DISPLAY_MODEL
  ESP_LOGD(TAG, "Display      : %s", disp ? "+" : "-");
#endif

#ifdef HAS_SDCARD
  ESP_LOGD(TAG, "Sd Card      : %s", SD.cardSize() != 0 ? "+" : "-");
#endif

#ifdef HAS_PMU
  ESP_LOGD(TAG, "Power        : %s", PMU ? "+" : "-");
#endif

#ifdef HAS_GPS
  ESP_LOGD(TAG, "GPS          : %s", find_gps ? "+" : "-");
#endif

#ifdef DISPLAY_MODEL
  if (disp)
  {

    disp->clearBuffer();
    disp->setFont(u8g2_font_NokiaLargeBold_tf);
    uint16_t str_w = disp->getStrWidth(BOARD_VARIANT_NAME);
    disp->drawStr((disp->getWidth() - str_w) / 2, 16, BOARD_VARIANT_NAME);
    disp->drawHLine(5, 21, disp->getWidth() - 5);

    disp->drawStr(0, 38, "Disp:");
    disp->drawStr(45, 38, (disp) ? "+" : "-");

#ifdef HAS_SDCARD
    disp->drawStr(0, 54, "SD :");
    disp->drawStr(45, 54, (SD.cardSize() != 0) ? "+" : "-");
#endif

    disp->drawStr(62, 38, "Radio:");
    disp->drawStr(120, 38, (radioOnline) ? "+" : "-");

#ifdef HAS_PMU
    disp->drawStr(62, 54, "Power:");
    disp->drawStr(120, 54, (PMU) ? "+" : "-");
#endif

    disp->sendBuffer();

    delay(2000);
  }
#endif
#endif // DISPLAY_MODEL
}

bool LgLoraBoard::beginRadioLib(bool restartOnFail)
{
#if defined(USE_SX1262) || defined(USE_SX1276)
  int state = radio.begin();
  if (state != RADIOLIB_ERR_NONE)
  {
    ESP_LOGE(TAG, "LoRa initialization failed, code: %d", state);
    if (restartOnFail)
    {
      delay(5000);
      ESP.restart();
    }
    return false;
  }
  ESP_LOGD(TAG, "LoRa initialized successfully");

  if (radio.setFrequency(CONFIG_RADIO_FREQ) == RADIOLIB_ERR_INVALID_FREQUENCY)
  {
    ESP_LOGE(TAG, "Selected frequency is invalid for this module");
    if (restartOnFail)
    {
      delay(5000);
      ESP.restart();
    }
    return false;
  }

  // meshcore 62.5 kHz
  if (radio.setBandwidth(CONFIG_RADIO_BW) == RADIOLIB_ERR_INVALID_BANDWIDTH)
  {
    ESP_LOGE(TAG, "Selected bandwidth is invalid for this module");
    if (restartOnFail)
    {
      delay(5000);
      ESP.restart();
    }
    return false;
  }

  // set spreading factor to 8
  if (radio.setSpreadingFactor(CONFIG_RADIO_SF) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR)
  {
    ESP_LOGE(TAG, "Selected spreading factor is invalid for this module");
    if (restartOnFail)
    {
      delay(5000);
      ESP.restart();
    }
    return false;
  }

  // set coding rate to 8
  if (radio.setCodingRate(CONFIG_RADIO_CR) == RADIOLIB_ERR_INVALID_CODING_RATE)
  {
    ESP_LOGE(TAG, "Selected coding rate is invalid for this module");
    if (restartOnFail)
    {
      delay(5000);
      ESP.restart();
    }
    return false;
  }

  // Secure set LoRa sync word
  if (radio.setSyncWord(CONFIG_RADIO_SYNC_WORD) != RADIOLIB_ERR_NONE)
  {
    ESP_LOGE(TAG, "Unable to set sync word!");
    if (restartOnFail)
    {
      delay(5000);
      ESP.restart();
    }
    return false;
  }

  // // set output power to 22 dBm (accepted range is -17 - 22 dBm)
  // if (radio.setOutputPower(CONFIG_RADIO_OUTPUT_POWER) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
  //   ESP_LOGE(TAG, "Selected output power is invalid for this module!");
  //   while (true) { delay(10); }
  // }

  // // set over current protection limit to 80 mA (accepted range is 45 - 240 mA)
  // // NOTE: set value to 0 to disable overcurrent protection
  // if (radio.setCurrentLimit(CONFIG_RADIO_CURRENT_LIMIT) == RADIOLIB_ERR_INVALID_CURRENT_LIMIT) {
  //   ESP_LOGE(TAG, "Selected current limit is invalid for this module!");
  //   while (true) { delay(10); }
  // }

  // // set LoRa preamble length to 15 symbols (accepted range is 0 - 65535)
  // if (radio.setPreambleLength(CONFIG_RADIO_PREAMBLE_LENGTH) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
  //   ESP_LOGE(TAG, "Selected preamble length is invalid for this module");
  //   while (true) { delay(10); }
  // }

  // // disable CRC
  // if (radio.setCRC(CONFIG_RADIO_CRC) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION) {
  //   ESP_LOGE(TAG, "Selected CRC is invalid for this module");
  //   while (true) { delay(10); }
  // }

  // // Some SX126x modules have TCXO (temperature compensated crystal
  // // oscillator). To configure TCXO reference voltage,
  // // the following method can be used.
  // if (radio.setTCXO(2.4) == RADIOLIB_ERR_INVALID_TCXO_VOLTAGE) {
  //   ESP_LOGE(TAG, "Selected TCXO voltage is invalid for this module!");
  //   while (true) { delay(10); }
  // }

  // // Some SX126x modules use DIO2 as RF switch. To enable
  // // this feature, the following method can be used.
  // // NOTE: As long as DIO2 is configured to control RF switch,
  // //       it can't be used as interrupt pin!
  // if (radio.setDio2AsRfSwitch() != RADIOLIB_ERR_NONE) {
  //   ESP_LOGE(TAG, "Failed to set DIO2 as RF switch!");
  //   while (true) { delay(10); }
  // }

  return true;
#else
  return false;
#endif // defined(USE_SX1262) || defined(USE_SX1276)
}

bool LgLoraBoard::setupRadio(bool restartOnFail)
{
#if defined(USE_SX1262) || defined(USE_SX1276)
  if (!beginRadioLib(restartOnFail))
  {
    return false;
  }

  ESP_LOGD(TAG, "Radio is ready!");

  // set the function that will be called
  // when new packet is received
  radio.setPacketReceivedAction(setFlag);

  // start listening for LoRa packets
  ESP_LOGD(TAG, "Starting to listen ... ");
  int16_t state = radio.startReceive();
  if (state != RADIOLIB_ERR_NONE)
  {
    ESP_LOGE(TAG, "Failed, code: %d", state);
    if (restartOnFail)
    {
      delay(5000);
      ESP.restart();
    }
    return false;
  }
  ESP_LOGD(TAG, "Started receive");
  return true;
#else
  return beginLoraLib(restartOnFail);
#endif // USE_SX1262 || USE_SX1276
}

int LgLoraBoard::hasRadio()
{
#if defined(USE_SX1262) || defined(USE_SX1276)
  return receivedFlag ? radio.getPacketLength() : 0;
// #else
//   return LoRa.parsePacket();
#endif // USE_SX1262 || USE_SX1276
}

void LgLoraBoard::readRadioBytes(int packetSize, byte *data)
{
#if defined(USE_SX1262) || defined(USE_SX1276)
  if (receivedFlag)
  {
    receivedFlag = false;

    int state = radio.readData(data, packetSize);
    if (state == RADIOLIB_ERR_NONE)
    {
      // packet was successfully received
      ESP_LOGD(TAG, "Received packet!");

      // Serial.print(str);

      // // print RSSI (Received Signal Strength Indicator)
      // Serial.print(F("RSSI:\t\t"));
      // Serial.print(radio.getRSSI());
      // Serial.println(F(" dBm"));

      // // print SNR (Signal-to-Noise Ratio)
      // Serial.print(F("SNR:\t\t"));
      // Serial.print(radio.getSNR());
      // Serial.println(F(" dB"));

      // // print frequency error
      // Serial.print(F("Frequency error:\t"));
      // Serial.print(radio.getFrequencyError());
      // Serial.println(F(" Hz"));
    }
    else if (state == RADIOLIB_ERR_CRC_MISMATCH)
    {
      // packet was received, but is malformed
      ESP_LOGE(TAG, "CRC error!");
    }
    else
    {
      // some other error occurred
      ESP_LOGE(TAG, "Failed to receive data, code %d", state);
    }
  }
// #else
//     if (packetSize) {
//         int i = 0;
//         while (LoRa.available() && i < packetSize) {
//             data[i++] = LoRa.read();
//         }
//         ESP_LOGD(TAG, "Received packet!");
//     }
#endif // USE_SX1262 || USE_SX1276
}

int LgLoraBoard::transmitRadioBytes(int packetSize, byte *data)
{
  int state = radio.transmit(data, packetSize);
  if (state == RADIOLIB_ERR_NONE)
  {
    // packet was successfully transmitted
    ESP_LOGD(TAG, "Packet transmitted!");
  }
  else
  {
    // some other error occurred
    ESP_LOGE(TAG, "Failed to transmit data, code %d", state);
  }

  return state;
}

void LgLoraBoard::setupButton(callbackFunction onClick, callbackFunction onDoubleClick, callbackFunction onLongPress)
{
#ifdef HAS_BUTTON
  button->setupButton(onClick, onDoubleClick, onLongPress);
#else
  ESP_LOGW(TAG, "Button not available on this board");
#endif
}

void LgLoraBoard::updateOnlineStatus(uint32_t status)
{
  deviceOnline |= status;
}

void LgLoraBoard::flashLed(uint32_t debounceDelay)
{
#ifdef HAS_LED
  led->flashLed(debounceDelay);
#else
  ESP_LOGW(TAG, "LED not available on this board");
#endif // HAS_LED
}
