/**
 * LgDisplay.h - Header file for the LgDisplay class, which provides an interface for initializing and controlling the display on the LilyGo boards.
 * Author: mcuw (112967559+mcuw@users.noreply.github.com)
 * License: MIT
 * Copyright (c) 2026 mcuw
 * Date: 2026-04-02
 * Last update: 2026-04-02
 *
 * This class is responsible for initializing the display, showing a splash screen, and providing an interface for future display-related functions.
 * Options:
 * - HIDE_SPLASH_SCREEN: Define this macro to skip displaying the splash screen during initialization.
 */

#ifndef LG_DISPLAY_H
#define LG_DISPLAY_H

#ifndef DISPLAY_MODEL
    class SSD1306Wire;
#else
    class DISPLAY_MODEL;
#endif // DISPLAY_MODEL

#ifdef HAS_DISPLAY_SSD1306
class LgDisplay
{
public:
    LgDisplay();
    virtual ~LgDisplay();
    bool beginDisplay();

private:
    void displaySplashScreen();
#ifndef DISPLAY_MODEL
    SSD1306Wire* display;
#else
    DISPLAY_MODEL* display;
#endif // DISPLAY_MODEL

};
#endif // HAS_DISPLAY_SSD1306

#endif /* LG_DISPLAY_H */