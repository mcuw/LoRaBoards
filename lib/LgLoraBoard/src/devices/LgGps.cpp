#include <Arduino.h>
#include "LgGps.h"

#ifdef HAS_GPS

static const char *TAG = "LgGps";

bool find_gps = false;
String gps_model = "None";

bool beginGPS()
{
    SerialGPS.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    bool result = false;
    for (int i = 0; i < 3; ++i)
    {
        result = l76kProbe();
        if (result)
        {
            return result;
        }
    }
    return result;
}

bool l76kProbe()
{
    bool result = false;
    uint32_t startTimeout;
    SerialGPS.write("$PCAS03,0,0,0,0,0,0,0,0,0,0,,,0,0*02\r\n");
    delay(5);
    // Get version information
    startTimeout = millis() + 3000;
    ESP_LOGD(TAG, "Try to init L76K . Wait stop .");
    // SerialGPS.flush();
    while (SerialGPS.available())
    {
        int c = SerialGPS.read();
        // ESP_LOGD(TAG, c, ".");
        // Serial.flush();
        // SerialGPS.flush();
        if (millis() > startTimeout)
        {
            ESP_LOGD(TAG, "Wait L76K stop NMEA timeout!");
            return false;
        }
    };
    SerialGPS.flush();
    delay(200);

    SerialGPS.write("$PCAS06,0*1B\r\n");
    startTimeout = millis() + 500;
    String ver = "";
    while (!SerialGPS.available())
    {
        if (millis() > startTimeout)
        {
            ESP_LOGD(TAG, "Get L76K timeout!");
            return false;
        }
    }
    SerialGPS.setTimeout(10);
    ver = SerialGPS.readStringUntil('\n');
    if (ver.startsWith("$GPTXT,01,01,02"))
    {
        ESP_LOGD(TAG, "L76K GNSS init succeeded, using L76K GNSS Module\n");
        result = true;
    }
    delay(500);

    // Initialize the L76K Chip, use GPS + GLONASS
    SerialGPS.write("$PCAS04,5*1C\r\n");
    delay(250);
    // only ask for RMC and GGA
    SerialGPS.write("$PCAS03,1,0,0,0,1,0,0,0,0,0,,,0,0*02\r\n");
    delay(250);
    // Switch to Vehicle Mode, since SoftRF enables Aviation < 2g
    SerialGPS.write("$PCAS11,3*1E\r\n");
    return result;
}

bool recoveryGPS()
{
    uint8_t buffer[256];
    uint8_t cfg_clear1[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x1C, 0xA2};
    uint8_t cfg_clear2[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1B, 0xA1};
    uint8_t cfg_clear3[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x03, 0x1D, 0xB3};

    SerialGPS.write(cfg_clear1, sizeof(cfg_clear1));
    if (getAck(buffer, 256, 0x05, 0x01))
    {
        ESP_LOGD(TAG, "ACK clear1");
    }

    SerialGPS.write(cfg_clear2, sizeof(cfg_clear2));
    if (getAck(buffer, 256, 0x05, 0x01))
    {
        ESP_LOGD(TAG, "ACK clear2");
    }

    SerialGPS.write(cfg_clear3, sizeof(cfg_clear3));
    if (getAck(buffer, 256, 0x05, 0x01))
    {
        ESP_LOGD(TAG, "ACK clear3");
    }

    // UBX-CFG-RATE, Size 8, 'Navigation/measurement rate settings'
    uint8_t cfg_rate[] = {0xB5, 0x62, 0x06, 0x08, 0x00, 0x00, 0x0E, 0x30};
    SerialGPS.write(cfg_rate, sizeof(cfg_rate));
    if (getAck(buffer, 256, 0x06, 0x08))
    {
        ESP_LOGD(TAG, "ACK cfg_rate");
    }
    else
    {
        return false;
    }
    return true;
}

static int getAck(uint8_t *buffer, uint16_t size, uint8_t requestedClass, uint8_t requestedID)
{
    uint16_t ubxFrameCounter = 0;
    bool ubxFrame = 0;
    uint32_t startTime = millis();
    uint16_t needRead;

    while (millis() - startTime < 800)
    {
        while (SerialGPS.available())
        {
            int c = SerialGPS.read();
            switch (ubxFrameCounter)
            {
            case 0:
                if (c == 0xB5)
                {
                    ubxFrameCounter++;
                }
                break;
            case 1:
                if (c == 0x62)
                {
                    ubxFrameCounter++;
                }
                else
                {
                    ubxFrameCounter = 0;
                }
                break;
            case 2:
                if (c == requestedClass)
                {
                    ubxFrameCounter++;
                }
                else
                {
                    ubxFrameCounter = 0;
                }
                break;
            case 3:
                if (c == requestedID)
                {
                    ubxFrameCounter++;
                }
                else
                {
                    ubxFrameCounter = 0;
                }
                break;
            case 4:
                needRead = c;
                ubxFrameCounter++;
                break;
            case 5:
                needRead |= (c << 8);
                ubxFrameCounter++;
                break;
            case 6:
                if (needRead >= size)
                {
                    ubxFrameCounter = 0;
                    break;
                }
                if (SerialGPS.readBytes(buffer, needRead) != needRead)
                {
                    ubxFrameCounter = 0;
                }
                else
                {
                    return needRead;
                }
                break;

            default:
                break;
            }
        }
    }
    return 0;
}

#endif /* HAS_GPS */