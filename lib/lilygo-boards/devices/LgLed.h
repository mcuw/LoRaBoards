#ifndef LG_LED_H
#define LG_LED_H
#include <Arduino.h>

#ifdef HAS_LED
class LgLed
{
public:
  LgLed();
  virtual ~LgLed() {};
  
  void flashLed(uint32_t debounceDelay = 50);
private:
  static uint8_t ledState;
  static uint32_t lastDebounceTime;
};
#endif

#endif // LG_LED_H