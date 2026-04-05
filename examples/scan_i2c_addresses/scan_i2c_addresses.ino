// Scan I2C addresses for troubleshooting I2C devices.
#include <Arduino.h>
#include <Wire.h>

#define WIRE Wire

void scanDevices(TwoWire *wire)
{
    byte error, address;
    int nDevices = 0;

    Serial.println(F("Scanning..."));

    for (address = 1; address < 127; address++)
    {
        wire->beginTransmission(address);
        error = wire->endTransmission();

        if (error == 0)
        {
            Serial.print(F("I2C device found at address: 0x"));
            if (address < 16)
                Serial.print(F("0"));
            Serial.println(address, HEX);

            nDevices++;
        }
        else if (error == 4)
        {
            Serial.print(F("Unknown error at address: 0x"));
            if (address < 16)
                Serial.print(F("0"));
            Serial.println(address, HEX);
        }
    }
    if (nDevices == 0)
        Serial.println(F("No I2C devices found\n"));
    else
        Serial.println(F("done\n"));
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("Scan I2C example"));

  WIRE.begin();
}

void loop() {
  Serial.println(F("================== Scan Wire =================="));
  scanDevices(&WIRE);
  delay(5000);
}
