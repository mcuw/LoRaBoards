#ifndef LG_TWOWIRE_H
#define LG_TWOWIRE_H

#include <Wire.h>

void scanDevices(TwoWire *w);

extern uint8_t  bme280_address;     // It might be 0x76
extern uint8_t  mag_address;        // QMC6310U=0x1C QMC6310N=0x3C
extern uint8_t  display_address;    // It might be 0x3C or 0x3D

#endif /* LG_TWOWIRE_H */