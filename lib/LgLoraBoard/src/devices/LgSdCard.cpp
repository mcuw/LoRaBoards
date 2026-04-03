#ifdef HAS_SDCARD
#include <Arduino.h>
#include <FS.h>
#include <SPI.h>
#include "LgSdCardDevice.h"


static const char *TAG = "LgSdCardDevice";

bool writeFile(const char *path, const char *buffer)
{
    bool rlst = false;
    File file = SD.open(path, FILE_WRITE);
    if (!file) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return false;
    }
    if (file.print(buffer)) {
        ESP_LOGD(TAG, "File written");
        rlst = true;
    } else {
        ESP_LOGE(TAG, "Write failed");
        rlst = false;
    }
    file.close();
    return  rlst;
}

bool readFile(const char *path, uint8_t *buffer, size_t size)
{
    File file = SD.open(path, FILE_READ);
    if (!file) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return false;
    }
    file.read(buffer, size);
    file.close();
    return false;
}

bool testSDWriteAndRead()
{
    const char *path = "/test_sd.txt";
    const char *message = "This is a string for reading and writing SD card.";
    uint8_t buffer[128] = {0};

    if (!writeFile(path, message)) {
        ESP_LOGE(TAG, "SD Text write failed");
        return false;
    }
    delay(100);

    readFile(path, buffer, 128);

    if (memcmp(buffer, message, strlen(message)) != 0) {
        ESP_LOGE(TAG, "SD verification failed");
        return false;
    }
    ESP_LOGD(TAG, "SD verification successful");
    return true;
}

bool beginSDCard()
{
#ifdef SD_SHARE_SPI_BUS
    bool rlst = SD.begin(SDCARD_CS);
#else
    bool rlst = SD.begin(SDCARD_CS, SDCardSPI);
#endif

    if (rlst) {
        uint32_t cardSize = SD.cardSize() / (1024 * 1024);
        ESP_LOGI(TAG, "Sd Card init succeeded, The current available capacity is %.2f GB", cardSize / 1024.0);
        deviceOnline |= SDCARD_ONLINE;
        return testSDWriteAndRead();
    } else {
        ESP_LOGE(TAG, "Warning: Failed to init Sd Card");
    }
    return false;
}

#endif // HAS_SDCARD
