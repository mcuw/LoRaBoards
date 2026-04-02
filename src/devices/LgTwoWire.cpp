#include <Arduino.h>
#include "LgLoraBoard.h"
#include "LgTwoWire.h"

static const char *TAG = "LgTwoWire";

// I2S Devices default address
//TODO define in config.h
uint8_t  bme280_address = 0x77;     // It might be 0x76
uint8_t  mag_address = 0x1C;        // QMC6310U=0x1C QMC6310N=0x3C
uint8_t  display_address = 0x3c;    // It might be 0x3D

void scanDevices(TwoWire *w)
{
    uint8_t err, addr;
    int nDevices = 0;
    uint32_t start = 0;

    ESP_LOGI(TAG, "I2C Devices scanning");
    for (addr = 1; addr < 127; addr++)
    {
        start = millis();
        w->beginTransmission(addr);
        delay(2);
        err = w->endTransmission();
        if (err == 0)
        {
            nDevices++;
            switch (addr)
            {
            case 0x77:
                bme280_address = addr;
                ESP_LOGI(TAG, "\tFound BME280 Sensor at address 0x%02X", addr);
                LgLoraBoard::updateOnlineStatus(BME280_ONLINE);
                break;
            case 0x76:
                bme280_address = addr;
                ESP_LOGI(TAG, "\tFound BME280 Sensor at address 0x%02X", addr);
                LgLoraBoard::updateOnlineStatus(BME280_ONLINE);
                break;
            case 0x34:
                ESP_LOGI(TAG, "\tFound AXP192/AXP2101 PMU at address 0x%02X", addr);
                LgLoraBoard::updateOnlineStatus(POWERMANAGE_ONLINE);
                break;
            case 0x3C:
            case 0x3D:
            {
                w->beginTransmission(addr);
                w->write((uint8_t)0x00);
                w->endTransmission();
                w->requestFrom((int)addr, 1);
                uint8_t r = w->read();
                if (r == 0x80)
                {
                    ESP_LOGI(TAG, "\tFound QMC6310N MAG Sensor at address 0x%02X", addr);
                    mag_address = addr;
                    LgLoraBoard::updateOnlineStatus(QMC6310N_ONLINE);
                }
                else
                {
                    ESP_LOGI(TAG, "\tFound OLED display at address 0x%02X", addr);
                    display_address = addr;
                    LgLoraBoard::updateOnlineStatus(DISPLAY_ONLINE);
                }
            }
            break;
            case 0x51:
                ESP_LOGI(TAG, "\tFound PCF8563 RTC at address 0x%02X", addr);
                LgLoraBoard::updateOnlineStatus(PCF8563_ONLINE);
                break;
            case 0x1C:
                ESP_LOGI(TAG, "\tFound QMC6310U MAG Sensor at address 0x%02X", addr);
                LgLoraBoard::updateOnlineStatus(QMC6310U_ONLINE);
                mag_address = addr;
                break;
            default:
                ESP_LOGI(TAG, "\tI2C device found at address 0x%02X", addr);
                break;
            }
        }
        else if (err == 4)
        {
            ESP_LOGI(TAG, "Unknown error at address 0x%02X", addr);
            if (addr < 16)
            {
                ESP_LOGI(TAG, "0");
            }
            ESP_LOGI(TAG, "%02X", addr);
        }
    }
    if (nDevices == 0)
        ESP_LOGI(TAG, "No I2C devices found\n");
}
