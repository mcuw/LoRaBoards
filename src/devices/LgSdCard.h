#ifndef LG_SD_CARD_H
#define LG_SD_CARD_H

#ifdef HAS_SDCARD
#include <SD.h>
bool beginSDCard();
extern SPIClass SDCardSPI;

#else
#define beginSDCard()
#endif



#endif /* LG_SD_CARD_H */