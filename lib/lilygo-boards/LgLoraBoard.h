/**
 * @file        LgLoraBoard.h
 * @author      mcuw (112967559+mcuw@users.noreply.github.com)
 * @license     MIT
 * @copyright   Copyright (c) 2026 mcuw
 * @date        2026-03-26
 * @last-update 2026-03-26
 */
// ref https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/blob/master/examples/TBeamFactory/TBeamFactory.ino

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
class LgLed;

class LgLoraBoard {
public:
    LgLoraBoard();
    virtual ~LgLoraBoard();

    static void updateOnlineStatus(uint32_t status);

    void setupRadioBoard();

    // *** LoRa ***
    // LoRa setup
    bool setupRadio(bool restartOnFail = true);
    bool beginRadioLib(bool restartOnFail = true);
    // LoRa transactions
    int hasRadio();
    void readRadioBytes(int packetSize, byte* data);
    int transmitRadioBytes(int packetSize, byte* data);

    // *** Board Status ***
    void printBoardStatus();
    void printWakeupReason();
    void printChipInfo();

    // *** Button ***
    void setupButton(callbackFunction onClick, callbackFunction onDoubleClick = nullptr, callbackFunction onLongPress = nullptr);

    // *** builtin LED ***
    void flashLed(uint32_t debounceDelay = 50);

protected:

private:
    static uint32_t deviceOnline;
    LgButton* button;
    LgLed* led;
};

#endif /* LG_LORA_BOARD_H */