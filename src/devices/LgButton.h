#ifndef LG_BUTTON_H
#define LG_BUTTON_H

#ifdef HAS_BUTTON
#include <OneButton.h>
#include "../types/LgButtonTypes.h"

class LgButton {
public:
    // initialization with config.h (BUTTON_PIN, BUTTON_ACTIVEHIGH and BUTTON_PULLUP)
    LgButton();
    virtual ~LgButton();

    void setupButton(callbackFunction onClickInterrupt, callbackFunction onDoubleClickInterrupt = nullptr, callbackFunction onLongPressInterrupt = nullptr);
    
    // optional: reconfigure the button with a new settings
    // mode: can be INPUT, INPUT_PULLUP
    void reconfigureButton(uint8_t pin, uint8_t mode = INPUT_PULLUP, bool activeLow = true);

private:
    static void buttonTick(void *pvParameters);

    OneButton* button;
    TaskHandle_t taskButtonHandle;
};
#endif // HAS_BUTTON
#endif // LG_BUTTON_H