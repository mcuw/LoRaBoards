#ifdef HAS_BUTTON
#include <Arduino.h>
#include "config.h"
#include "LgButton.h"

OneButton button(BUTTON_PIN, !BUTTON_ACTIVEHIGH, !!BUTTON_PULLUP); // pin, active low, use internal pullup
TaskHandle_t taskButtonHandle;

void buttonTick(void *pvParameters)
{
  for (;;)
  {
    button.tick();
    vTaskDelay(CONFIG_BUTTON_TICK_INTERVAL_MS / portTICK_PERIOD_MS);
  }
}

void setupButton(callbackFunction onClick, callbackFunction onDoubleClick, callbackFunction onLongPress)
{
  button.setup(BUTTON_PIN, INPUT_PULLUP);

  button.attachClick(onClick);
  button.attachDoubleClick(onDoubleClick);
  button.attachLongPressStart(onLongPress);

  xTaskCreatePinnedToCore(
      buttonTick,                  // Function name of the task
      "Builtin Button",                  // Name of the task (e.g. for debugging)
      1024,                        // Stack size (bytes)
      NULL,                        // Parameter to pass
      CONFIG_BUTTON_TASK_PRIORITY, // Task priority
      &taskButtonHandle,           // Assign task handle
      CONFIG_BUTTON_CORE           // Run on core 1
  );
}
#endif // HAS_BUTTON