#ifndef LG_GPS_H
#define LG_GPS_H

#ifdef HAS_GPS
bool beginGPS();
bool l76kProbe();
static int getAck(uint8_t *buffer, uint16_t size, uint8_t requestedClass, uint8_t requestedID);

bool recoveryGPS();

extern bool find_gps;
extern String gps_model;

#define SerialGPS Serial1

#endif /* HAS_GPS */

#endif /* LG_GPS_H */