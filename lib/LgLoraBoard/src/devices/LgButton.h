#ifndef LG_BUTTON_H
#define LG_BUTTON_H

#ifdef HAS_BUTTON
#include <OneButton.h>
#include "../types/LgButtonTypes.h"

class LgButton {
public:
    LgButton();
    virtual ~LgButton();

    void setupButton(callbackFunction onClickInterrupt, callbackFunction onDoubleClickInterrupt = nullptr, callbackFunction onLongPressInterrupt = nullptr);

private:
    static void buttonTick(void *pvParameters);

    OneButton* button;
    TaskHandle_t taskButtonHandle;
};
#endif // HAS_BUTTON
#endif // LG_BUTTON_H