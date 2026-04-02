#ifndef LG_BLUETOOTH_H
#define LG_BLUETOOTH_H

#ifdef HAS_BLE
void setupBLE();
#else
#define setupBLE()
#endif

#endif /* LG_BLUETOOTH_H */