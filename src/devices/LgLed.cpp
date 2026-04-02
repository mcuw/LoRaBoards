#ifdef HAS_LED
#include <Arduino.h>
#include "LgLed.h"
#include "config.h"

#ifndef LED_ON
#define LED_ON HIGH // LED to high level is turn on
#endif // LED_ON

LgLed::LgLed() :
  _ledState(!LED_ON),
  _lastDebounceTime(0),
  _enabled(false),
  _debounceDelay(0)
{
}

void LgLed::loopLed(void *pvParameters)
{
  LgLed *self = static_cast<LgLed *>(pvParameters);

  for (;;) {
    if (self->_enabled) {
      if ((millis() - self->_lastDebounceTime) > self->_debounceDelay)
      {
        self->_ledState = !self->_ledState;
        if (self->_ledState)
        {
          digitalWrite(LED_BUILTIN, LED_ON);
        }
        else
        {
          digitalWrite(LED_BUILTIN, !LED_ON);
        }
        self->_lastDebounceTime = millis();
      }
    }

    vTaskDelay(CONFIG_LED_TICK_INTERVAL_MS / portTICK_PERIOD_MS);
  }
}

void LgLed::setupLed()
{
  #if LED_ON == LOW
  // some T-Beam LED defaults to low level as turn on,
  // so it needs to be forced to pull up
  gpio_hold_dis((gpio_num_t)LED_BUILTIN);
#endif // LED_ON == LOW

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, _ledState);

  xTaskCreatePinnedToCore(
    loopLed,                  // Function name of the task
    "Builtin LED",            // Name of the task (e.g. for debugging)
    1024,                     // Stack size (bytes)
    this,                     // Parameter to pass
    CONFIG_LED_TASK_PRIORITY, // Task priority
    &taskLedHandle,           // Assign task handle
    CONFIG_LED_CORE           // Run on core 1
  );
}

void LgLed::enableBlinkLed(bool enable, uint32_t debounceDelay)
{
  _enabled = enable;
  _debounceDelay = debounceDelay;

  if (!enable)
  {
    // turn off LED when disabling
    digitalWrite(LED_BUILTIN, !LED_ON);
  }
}

#endif // HAS_LED