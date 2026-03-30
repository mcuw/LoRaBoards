#ifdef HAS_LED
#include <Arduino.h>
#include "LgLed.h"

uint8_t LgLed::ledState = LOW;
uint32_t LgLed::lastDebounceTime = 0;

LgLed::LgLed()
{
}

void LgLed::flashLed(uint32_t debounceDelay)
{
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    ledState = !ledState;
    if (ledState)
    {
      digitalWrite(LED_BUILTIN, LED_ON);
    }
    else
    {
      digitalWrite(LED_BUILTIN, !LED_ON);
    }
    lastDebounceTime = millis();
  }
}

#endif /* HAS_LED */