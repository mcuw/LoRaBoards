#ifdef HAS_BLE

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

void setupBLE()
{

    uint8_t mac[6];
    char macStr[18] = { 0 };
    esp_efuse_mac_get_default(mac);
    sprintf(macStr, "%02X:%02X", mac[0], mac[1]);

    String dev = BOARD_VARIANT_NAME;
    dev.concat('-');
    dev.concat(macStr);

    ESP_LOGD(TAG, "Starting BLE: %s", dev.c_str());

    BLEDevice::init(dev.c_str());
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE);

    pCharacteristic->setValue("Hello World");
    pService->start();
    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    ESP_LOGD(TAG, "Characteristic defined! Now you can read it in your phone!");
}
#endif