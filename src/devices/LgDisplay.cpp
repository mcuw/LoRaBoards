#ifdef HAS_DISPLAY_SSD1306
#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "LgDisplay.h"

#ifndef DISPLAY_MODEL
#include "SSD1306Wire.h"
#define DISPLAY_MODEL SSD1306Wire
#endif // DISPLAY_MODEL

static const char *TAG = "LgDisplay";

LgDisplay::LgDisplay()
{
    display = new DISPLAY_MODEL(DISPLAY_ADDR, OLED_SDA, OLED_SCL);
}

LgDisplay::~LgDisplay()
{
  if (display) {
    delete display;
    display = nullptr;
  }
}

bool LgDisplay::beginDisplay()
{
  ESP_LOGD(TAG, "initialize display");

  bool success = display->init();
  if (!success)
  {
    ESP_LOGE(TAG, "Failed to initialize display");
    return false;
  }

  display->clear();
  display->flipScreenVertically();

#ifndef HIDE_SPLASH_SCREEN
  displaySplashScreen();
#endif // HIDE_SPLASH_SCREEN

  return true;
}

void LgDisplay::displaySplashScreen() {
  display->setFont(ArialMT_Plain_16);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(display->getWidth() / 2, (display->getHeight() / 2) - 16, F("ESP32 LoRa"));
  display->display();
}

#endif // HAS_DISPLAY_SSD1306
