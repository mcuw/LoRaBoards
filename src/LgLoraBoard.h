/**
 * @file        LgLoraBoard.h
 * @author      mcuw (112967559+mcuw@users.noreply.github.com)
 * @license     MIT
 * @copyright   Copyright (c) 2026 mcuw
 * @date        2026-03-26
 * @last-update 2026-03-26
 */
// examples: https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/blob/master/examples/TBeamFactory/TBeamFactory.ino
// references: https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/blob/master/examples/TBeamFactory/LoRaBoards.cpp
// RadioLib for SX126x/ SX127x modules, see e.g. /.pio/libdeps/ttgo_t_beam_v1_1/RadioLib/src/modules

#ifndef LG_LORA_BOARD_H
#define LG_LORA_BOARD_H

#include "config.h"

#include "types/LgDevInfoTypes.h"
#include "types/LgButtonTypes.h"

// Todo: Refactor to class and remove here
#include "devices/LgBluetooth.h"
#include "devices/LgDisplay.h"
#include "devices/LgGps.h"
#include "devices/LgPmu.h"
#include "devices/LgNtc.h"
#include "devices/LgSdCard.h"
#include "devices/LgTwoWire.h"

class LgButton;
class LgDisplay;
class LgLed;

class LgLoraBoard {
public:
    static void updateOnlineStatus(uint32_t status);
    
    LgLoraBoard();
    virtual ~LgLoraBoard();

    // initialize the board and all devices (LoRa, GPS, Bluetooth, etc.)
    // depending on the board configuration defined in config.h
    void setupRadioBoard();

    // *** LoRa ***
    int hasRadioPacket();
    void readRadioBytes(int packetSize, byte* data);
    int transmitRadioBytes(int packetSize, byte* data);

    // *** Board Status ***
    void printBoardStatus();
    void printWakeupReason();
    void printChipInfo();

    // *** Button ***
    // Set callback functions for button interrupts, if supported by the board.
    // If the board doesn't support button interrupts, this function will do nothing.
    void setupButton(callbackFunction onClickInterrupt, callbackFunction onDoubleClickInterrupt = nullptr, callbackFunction onLongPressInterrupt = nullptr);

    // *** Builtin LED ***
    void blinkLed(uint32_t debounceDelay = 50);
    void disableLed();

protected:

private:
    // LoRa
    bool setupRadio(bool restartOnFail = true); // calls beginRadioLib and sets up receive callback
    bool beginRadioLib(bool restartOnFail = true); // initializes the radio with the settings defined in config.h, returns true if successful

    static uint32_t deviceOnline;
    LgButton* button;
    LgDisplay* display;
    LgLed* led;
};

#endif /* LG_LORA_BOARD_H */