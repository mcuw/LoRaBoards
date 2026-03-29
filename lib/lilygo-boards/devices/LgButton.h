#ifndef LG_BUTTON_H
#define LG_BUTTON_H

#ifdef HAS_BUTTON
#include <OneButton.h>

typedef void (*callbackFunction)(void);

void setupButton(callbackFunction onClick, callbackFunction onDoubleClick = nullptr, callbackFunction onLongPress = nullptr);

#endif // HAS_BUTTON
#endif // LG_BUTTON_H