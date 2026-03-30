#ifndef LG_LED_H
#define LG_LED_H

#ifdef HAS_LED
#include <Arduino.h>

class LgLed
{
public:
  LgLed();
  virtual ~LgLed() {};
  
  void setupLed();
  void enableBlinkLed(bool enable = false, uint32_t debounceDelay = 50);

private:
  static void loopLed(void *pvParameters);

  uint8_t ledState;
  uint32_t lastDebounceTime;
  bool _enabled;
  uint32_t _debounceDelay;
  TaskHandle_t taskLedHandle;
};
#endif // HAS_LED
#endif // LG_LED_H