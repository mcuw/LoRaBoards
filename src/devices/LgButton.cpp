#ifdef HAS_BUTTON
#include <Arduino.h>
#include "config.h"
#include "LgButton.h"

LgButton::LgButton()
    : button(new OneButton(BUTTON_PIN, BUTTON_ACTIVELOW, !!BUTTON_PULLUP)) // pin, active low, use internal pullup
{
};

LgButton::~LgButton()
{
  if (button)
  {
    delete button;
    button = nullptr;
  }
}

void LgButton::buttonTick(void *pvParameters)
{
  LgButton *self = static_cast<LgButton *>(pvParameters);

  for (;;)
  {
    self->button->tick();
    vTaskDelay(LORA_BUTTON_TICK_INTERVAL_MS / portTICK_PERIOD_MS);
  }
}

void LgButton::setupButton(callbackFunction onClickInterrupt, callbackFunction onDoubleClickInterrupt, callbackFunction onLongPressInterrupt)
{
  button->attachClick(onClickInterrupt);
  button->attachDoubleClick(onDoubleClickInterrupt);
  button->attachLongPressStart(onLongPressInterrupt);

  xTaskCreatePinnedToCore(
      buttonTick,                  // Function name of the task
      "Builtin Button",            // Name of the task (e.g. for debugging)
      1024,                        // Stack size (bytes)
      this,                        // Parameter to pass
      LORA_BUTTON_TASK_PRIORITY, // Task priority
      &taskButtonHandle,           // Assign task handle
      LORA_BUTTON_CORE           // Run on core 1
  );
}

void LgButton::reconfigureButton(uint8_t pin, uint8_t mode, bool activeLow)
{
  button->setup(pin, mode, activeLow);
}
#endif // HAS_BUTTON