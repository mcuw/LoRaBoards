#ifndef LG_PMU_H
#define LG_PMU_H

#ifdef HAS_PMU
#include <XPowersLib.h>

extern XPowersLibInterface *PMU;
extern bool pmuInterrupt;
void loopPMU(void (*pressed_cb)(void));
bool beginPower();
void disablePeripherals();
#else
#define beginPower()
#define disablePeripherals()
#endif

#endif /* LG_PMU_H */