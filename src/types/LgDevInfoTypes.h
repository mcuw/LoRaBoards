#include <Arduino.h>

#pragma once

enum {
    POWERMANAGE_ONLINE  = _BV(0),
    DISPLAY_ONLINE      = _BV(1),
    RADIO_ONLINE        = _BV(2),
    GPS_ONLINE          = _BV(3),
    PSRAM_ONLINE        = _BV(4),
    SDCARD_ONLINE       = _BV(5),
    AXDL345_ONLINE      = _BV(6),
    BME280_ONLINE       = _BV(7),
    BMP280_ONLINE       = _BV(8),
    BME680_ONLINE       = _BV(9),
    QMC6310U_ONLINE      = _BV(10),
    QMC6310N_ONLINE      = _BV(11),
    QMI8658_ONLINE      = _BV(12),
    PCF8563_ONLINE      = _BV(13),
    OSC32768_ONLINE      = _BV(14)
};

typedef struct {
    String          chipModel;
    float           psramSize;
    uint8_t         chipModelRev;
    uint8_t         chipFreq;
    uint8_t         flashSize;
    uint8_t         flashSpeed;
} DevInfo_t;

typedef void (*callbackFunction)(void);
