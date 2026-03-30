#ifdef HAS_LED
#include <Arduino.h>
#include "LgLed.h"
#include "config.h"

LgLed::LgLed() :
  ledState(LOW),
  lastDebounceTime(0),
  _enabled(false),
  _debounceDelay(0)
{
}

void LgLed::loopLed(void *pvParameters)
{
  LgLed *self = static_cast<LgLed *>(pvParameters);

  for (;;) {
    if (self->_enabled) {
      if ((millis() - self->lastDebounceTime) > self->_debounceDelay)
      {
        self->ledState = !self->ledState;
        if (self->ledState)
        {
          digitalWrite(LED_BUILTIN, LED_ON);
        }
        else
        {
          digitalWrite(LED_BUILTIN, !LED_ON);
        }
        self->lastDebounceTime = millis();
      }
    }

    vTaskDelay(CONFIG_LED_TICK_INTERVAL_MS / portTICK_PERIOD_MS);
  }
}

void LgLed::setupLed()
{
  // T-Beam LED defaults to low level as turn on,
  // so it needs to be forced to pull up
#if LED_ON == LOW
  gpio_hold_dis((gpio_num_t)LED_BUILTIN);
#endif // LED_ON == LOW
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_ON);

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
#ifdef HAS_LED
  _enabled = enable;
  _debounceDelay = debounceDelay;
#else
  ESP_LOGW(TAG, "LED not available on this board or not implemented yet");
#endif // HAS_LED
  }



#endif /* HAS_LED */