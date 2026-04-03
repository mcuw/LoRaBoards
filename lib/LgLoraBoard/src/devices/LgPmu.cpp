#include <Arduino.h>
#include "LgLoraBoard.h"

#ifdef HAS_PMU
XPowersLibInterface *PMU = NULL;
bool     pmuInterrupt;
static const char *TAG = "LgPmu";

static void setPmuFlag()
{
    pmuInterrupt = true;
}

bool beginPower()
{
    if (!PMU) {
        PMU = new XPowersAXP192(PMU_WIRE_PORT);
        if (!PMU->init()) {
            ESP_LOGW(TAG, "Warning: Failed to find AXP192 power management");
            delete PMU;
            PMU = NULL;
        } else {
            ESP_LOGD(TAG, "AXP192 PMU init succeeded, using AXP192 PMU");
        }
    }

    if (!PMU) {
        PMU = new XPowersAXP2101(PMU_WIRE_PORT);
        if (!PMU->init()) {
            ESP_LOGW(TAG, "Warning: Failed to find AXP2101 power management");
            delete PMU;
            PMU = NULL;
        } else {
            ESP_LOGD(TAG, "AXP2101 PMU init succeeded, using AXP2101 PMU");
        }
    }

    // If PMU is not found at all, return false
    if (!PMU) {
        return false;
    }

    LgLoraBoard::updateOnlineStatus(POWERMANAGE_ONLINE);

    PMU->setChargingLedMode(XPOWERS_CHG_LED_CTRL_CHG);

    pinMode(PMU_IRQ, INPUT_PULLUP);
    attachInterrupt(PMU_IRQ, setPmuFlag, FALLING);

    if (PMU->getChipModel() == XPOWERS_AXP192) {
        // lora
        PMU->setPowerChannelVoltage(XPOWERS_LDO2, 3300);
        // gps
        PMU->setPowerChannelVoltage(XPOWERS_LDO3, 3300);
        // oled
        PMU->setPowerChannelVoltage(XPOWERS_DCDC1, 3300);

        PMU->enablePowerOutput(XPOWERS_LDO2);
        PMU->enablePowerOutput(XPOWERS_LDO3);

        // protected oled power source
        PMU->setProtectedChannel(XPOWERS_DCDC1);
        // protected esp32 power source
        PMU->setProtectedChannel(XPOWERS_DCDC3);
        // enable oled power
        PMU->enablePowerOutput(XPOWERS_DCDC1);

        // disable not use channel
        PMU->disablePowerOutput(XPOWERS_DCDC2);

        PMU->disableIRQ(XPOWERS_AXP192_ALL_IRQ);

        PMU->enableIRQ(XPOWERS_AXP192_VBUS_REMOVE_IRQ |
                       XPOWERS_AXP192_VBUS_INSERT_IRQ |
                       XPOWERS_AXP192_BAT_CHG_DONE_IRQ |
                       XPOWERS_AXP192_BAT_CHG_START_IRQ |
                       XPOWERS_AXP192_BAT_REMOVE_IRQ |
                       XPOWERS_AXP192_BAT_INSERT_IRQ |
                       XPOWERS_AXP192_PKEY_SHORT_IRQ
                      );

    } else if (PMU->getChipModel() == XPOWERS_AXP2101) {

#ifdef CONFIG_IDF_TARGET_ESP32
        //Unuse power channel
        PMU->disablePowerOutput(XPOWERS_DCDC2);
        PMU->disablePowerOutput(XPOWERS_DCDC3);
        PMU->disablePowerOutput(XPOWERS_DCDC4);
        PMU->disablePowerOutput(XPOWERS_DCDC5);
        PMU->disablePowerOutput(XPOWERS_ALDO1);
        PMU->disablePowerOutput(XPOWERS_ALDO4);
        PMU->disablePowerOutput(XPOWERS_BLDO1);
        PMU->disablePowerOutput(XPOWERS_BLDO2);
        PMU->disablePowerOutput(XPOWERS_DLDO1);
        PMU->disablePowerOutput(XPOWERS_DLDO2);
        PMU->disablePowerOutput(XPOWERS_CPULDO);

        // GNSS RTC PowerVDD 3300mV
        PMU->setPowerChannelVoltage(XPOWERS_VBACKUP, 3300);
        PMU->enablePowerOutput(XPOWERS_VBACKUP);

        //ESP32 VDD 3300mV
        // ! No need to set, automatically open , Don't close it
        // PMU->setPowerChannelVoltage(XPOWERS_DCDC1, 3300);
        // PMU->setProtectedChannel(XPOWERS_DCDC1);
        PMU->setProtectedChannel(XPOWERS_DCDC1);

        // LoRa VDD 3300mV
        PMU->setPowerChannelVoltage(XPOWERS_ALDO2, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO2);

        //GNSS VDD 3300mV
        PMU->setPowerChannelVoltage(XPOWERS_ALDO3, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO3);

#endif /*CONFIG_IDF_TARGET_ESP32*/


#ifdef T_BEAM_S3_SUPREME
        //t-beam m.2 inface
        //gps
        PMU->setPowerChannelVoltage(XPOWERS_ALDO4, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO4);

        // lora
        PMU->setPowerChannelVoltage(XPOWERS_ALDO3, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO3);

        // In order to avoid bus occupation, during initialization, the SD card and QMC sensor are powered off and restarted
        if (ESP_SLEEP_WAKEUP_UNDEFINED == esp_sleep_get_wakeup_cause()) {
            ESP_LOGD(TAG, "Power off and restart ALDO BLDO..");
            PMU->disablePowerOutput(XPOWERS_ALDO1);
            PMU->disablePowerOutput(XPOWERS_ALDO2);
            PMU->disablePowerOutput(XPOWERS_BLDO1);
            delay(250);
        }

        // Sensor
        PMU->setPowerChannelVoltage(XPOWERS_ALDO1, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO1);

        PMU->setPowerChannelVoltage(XPOWERS_ALDO2, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO2);

        //Sdcard

        PMU->setPowerChannelVoltage(XPOWERS_BLDO1, 3300);
        PMU->enablePowerOutput(XPOWERS_BLDO1);

        PMU->setPowerChannelVoltage(XPOWERS_BLDO2, 3300);
        PMU->enablePowerOutput(XPOWERS_BLDO2);

        //face m.2
        PMU->setPowerChannelVoltage(XPOWERS_DCDC3, 3300);
        PMU->enablePowerOutput(XPOWERS_DCDC3);

        PMU->setPowerChannelVoltage(XPOWERS_DCDC4, XPOWERS_AXP2101_DCDC4_VOL2_MAX);
        PMU->enablePowerOutput(XPOWERS_DCDC4);

        PMU->setPowerChannelVoltage(XPOWERS_DCDC5, 3300);
        PMU->enablePowerOutput(XPOWERS_DCDC5);


        //not use channel
        PMU->disablePowerOutput(XPOWERS_DCDC2);
        // PMU->disablePowerOutput(XPOWERS_DCDC4);
        // PMU->disablePowerOutput(XPOWERS_DCDC5);
        PMU->disablePowerOutput(XPOWERS_DLDO1);
        PMU->disablePowerOutput(XPOWERS_DLDO2);
        PMU->disablePowerOutput(XPOWERS_VBACKUP);


#elifdef T_BEAM_S3_BPF
        //gps
        PMU->setPowerChannelVoltage(XPOWERS_ALDO4, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO4);

        //Sdcard
        PMU->setPowerChannelVoltage(XPOWERS_ALDO2, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO2);

        // Extern Power source
        PMU->setPowerChannelVoltage(XPOWERS_DCDC3, 3300);
        PMU->enablePowerOutput(XPOWERS_DCDC3);

        PMU->setPowerChannelVoltage(XPOWERS_DCDC5, 3300);
        PMU->enablePowerOutput(XPOWERS_DCDC5);

        PMU->setPowerChannelVoltage(XPOWERS_ALDO1, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO1);

        //not use channel
        PMU->disablePowerOutput(XPOWERS_BLDO1);
        PMU->disablePowerOutput(XPOWERS_BLDO2);
        PMU->disablePowerOutput(XPOWERS_DCDC4);
        PMU->disablePowerOutput(XPOWERS_DCDC2);
        PMU->disablePowerOutput(XPOWERS_DCDC4);
        PMU->disablePowerOutput(XPOWERS_DCDC5);
        PMU->disablePowerOutput(XPOWERS_DLDO1);
        PMU->disablePowerOutput(XPOWERS_DLDO2);
        PMU->disablePowerOutput(XPOWERS_VBACKUP);
#endif

        // Set constant current charge current limit
        PMU->setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_500MA);

        // Set charge cut-off voltage
        PMU->setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V2);

        // Disable all interrupts
        PMU->disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
        // Clear all interrupt flags
        PMU->clearIrqStatus();
        // Enable the required interrupt function
        PMU->enableIRQ(
            XPOWERS_AXP2101_BAT_INSERT_IRQ    | XPOWERS_AXP2101_BAT_REMOVE_IRQ      |   //BATTERY
            XPOWERS_AXP2101_VBUS_INSERT_IRQ   | XPOWERS_AXP2101_VBUS_REMOVE_IRQ     |   //VBUS
            XPOWERS_AXP2101_PKEY_SHORT_IRQ    | XPOWERS_AXP2101_PKEY_LONG_IRQ       |   //POWER KEY
            XPOWERS_AXP2101_BAT_CHG_DONE_IRQ  | XPOWERS_AXP2101_BAT_CHG_START_IRQ       //CHARGE
            // XPOWERS_AXP2101_PKEY_NEGATIVE_IRQ | XPOWERS_AXP2101_PKEY_POSITIVE_IRQ   |   //POWER KEY
        );

    }

    PMU->enableSystemVoltageMeasure();
    PMU->enableVbusVoltageMeasure();
    PMU->enableBattVoltageMeasure();

    ESP_LOGD(TAG, "=========================================");
    if (PMU->isChannelAvailable(XPOWERS_DCDC1)) {
        ESP_LOGD(TAG, "DC1  : %s   Voltage: %04u mV",  PMU->isPowerChannelEnable(XPOWERS_DCDC1)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC1));
    }
    if (PMU->isChannelAvailable(XPOWERS_DCDC2)) {
        ESP_LOGD(TAG, "DC2  : %s   Voltage: %04u mV",  PMU->isPowerChannelEnable(XPOWERS_DCDC2)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC2));
    }
    if (PMU->isChannelAvailable(XPOWERS_DCDC3)) {
        ESP_LOGD(TAG, "DC3  : %s   Voltage: %04u mV",  PMU->isPowerChannelEnable(XPOWERS_DCDC3)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC3));
    }
    if (PMU->isChannelAvailable(XPOWERS_DCDC4)) {
        ESP_LOGD(TAG, "DC4  : %s   Voltage: %04u mV",  PMU->isPowerChannelEnable(XPOWERS_DCDC4)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC4));
    }
    if (PMU->isChannelAvailable(XPOWERS_DCDC5)) {
        ESP_LOGD(TAG, "DC5  : %s   Voltage: %04u mV",  PMU->isPowerChannelEnable(XPOWERS_DCDC5)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC5));
    }
    if (PMU->isChannelAvailable(XPOWERS_LDO2)) {
        ESP_LOGD(TAG, "LDO2 : %s   Voltage: %04u mV",  PMU->isPowerChannelEnable(XPOWERS_LDO2)   ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_LDO2));
    }
    if (PMU->isChannelAvailable(XPOWERS_LDO3)) {
        ESP_LOGD(TAG, "LDO3 : %s   Voltage: %04u mV",  PMU->isPowerChannelEnable(XPOWERS_LDO3)   ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_LDO3));
    }
    if (PMU->isChannelAvailable(XPOWERS_ALDO1)) {
        ESP_LOGD(TAG, "ALDO1: %s   Voltage: %04u mV",  PMU->isPowerChannelEnable(XPOWERS_ALDO1)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_ALDO1));
    }
    if (PMU->isChannelAvailable(XPOWERS_ALDO2)) {
        ESP_LOGD(TAG, "ALDO2: %s   Voltage: %04u mV",  PMU->isPowerChannelEnable(XPOWERS_ALDO2)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_ALDO2));
    }
    if (PMU->isChannelAvailable(XPOWERS_ALDO3)) {
        ESP_LOGD(TAG, "ALDO3: %s   Voltage: %04u mV",  PMU->isPowerChannelEnable(XPOWERS_ALDO3)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_ALDO3));
    }
    if (PMU->isChannelAvailable(XPOWERS_ALDO4)) {
        ESP_LOGD(TAG, "ALDO4: %s   Voltage: %04u mV",  PMU->isPowerChannelEnable(XPOWERS_ALDO4)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_ALDO4));
    }
    if (PMU->isChannelAvailable(XPOWERS_BLDO1)) {
        ESP_LOGD(TAG, "BLDO1: %s   Voltage: %04u mV",  PMU->isPowerChannelEnable(XPOWERS_BLDO1)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_BLDO1));
    }
    if (PMU->isChannelAvailable(XPOWERS_BLDO2)) {
        ESP_LOGD(TAG, "BLDO2: %s   Voltage: %04u mV",  PMU->isPowerChannelEnable(XPOWERS_BLDO2)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_BLDO2));
    }


    // Set the time of pressing the button to turn off
    PMU->setPowerKeyPressOffTime(XPOWERS_POWEROFF_4S);
    uint8_t opt = PMU->getPowerKeyPressOffTime();
    ESP_LOGD(TAG, "PowerKeyPressOffTime:");
    switch (opt) {
    case XPOWERS_POWEROFF_4S: ESP_LOGD(TAG, "4 Second");
        break;
    case XPOWERS_POWEROFF_6S: ESP_LOGD(TAG, "6 Second");
        break;
    case XPOWERS_POWEROFF_8S: ESP_LOGD(TAG, "8 Second");
        break;
    case XPOWERS_POWEROFF_10S: ESP_LOGD(TAG, "10 Second");
        break;
    default:
        break;
    }

    ESP_LOGD(TAG, "=========================================");

    return true;
}

void disablePeripherals()
{

    if (!PMU)return;

    PMU->setChargingLedMode(XPOWERS_CHG_LED_OFF);
    // Disable the PMU measurement section
    PMU->disableSystemVoltageMeasure();
    PMU->disableVbusVoltageMeasure();
    PMU->disableBattVoltageMeasure();
    PMU->disableTemperatureMeasure();
    PMU->disableBattDetection();

#if defined(T_BEAM_S3_BPF)
    PMU->disablePowerOutput(XPOWERS_ALDO4); //gps
    PMU->disablePowerOutput(XPOWERS_ALDO2); //Sdcard
    PMU->disablePowerOutput(XPOWERS_DCDC3); // Extern Power source
    PMU->disablePowerOutput(XPOWERS_DCDC5);
    PMU->disablePowerOutput(XPOWERS_ALDO1);
#else

    if (PMU->getChipModel() == XPOWERS_AXP2101) {

        // Disable all PMU interrupts
        PMU->disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
        // Clear the PMU interrupt status before sleeping, otherwise the sleep current will increase
        PMU->clearIrqStatus();
        // GNSS RTC Power , Turning off GPS backup voltage and current can further reduce ~ 100 uA
        PMU->disablePowerOutput(XPOWERS_VBACKUP);
        // LoRa VDD
        PMU->disablePowerOutput(XPOWERS_ALDO2);
        // GNSS VDD
        PMU->disablePowerOutput(XPOWERS_ALDO3);

#if defined(T_BEAM_S3_SUPREME)
        PMU->disablePowerOutput(XPOWERS_ALDO4);
        PMU->disablePowerOutput(XPOWERS_ALDO1);
        PMU->disablePowerOutput(XPOWERS_BLDO1);
        PMU->disablePowerOutput(XPOWERS_BLDO2);
        PMU->disablePowerOutput(XPOWERS_DCDC3);
        PMU->disablePowerOutput(XPOWERS_DCDC4);
        PMU->disablePowerOutput(XPOWERS_DCDC5);
#endif

    } else if (PMU->getChipModel() == XPOWERS_AXP192) {

        // Disable all PMU interrupts
        PMU->disableIRQ(XPOWERS_AXP192_ALL_IRQ);
        // Clear the PMU interrupt status before sleeping, otherwise the sleep current will increase
        PMU->clearIrqStatus();
        // LoRa VDD
        PMU->disablePowerOutput(XPOWERS_LDO2);
        // GNSS VDD
        PMU->disablePowerOutput(XPOWERS_LDO3);


    }
#endif
}

void loopPMU(void (*pressed_cb)(void))
{
    if (!PMU) {
        return;
    }
    if (!pmuInterrupt) {
        return;
    }

    pmuInterrupt = false;
    // Get PMU Interrupt Status Register
    uint32_t status = PMU->getIrqStatus();
    ESP_LOGD(TAG, "STATUS => HEX: %08X BIN: %032b", status, status);

    if (PMU->isVbusInsertIrq()) {
        ESP_LOGD(TAG, "isVbusInsert");
    }
    if (PMU->isVbusRemoveIrq()) {
        ESP_LOGD(TAG, "isVbusRemove");
    }
    if (PMU->isBatInsertIrq()) {
        ESP_LOGD(TAG, "isBatInsert");
    }
    if (PMU->isBatRemoveIrq()) {
        ESP_LOGD(TAG, "isBatRemove");
    }
    if (PMU->isPekeyShortPressIrq()) {
        ESP_LOGD(TAG, "isPekeyShortPress");
        if (pressed_cb) {
            pressed_cb();
        }
    }
    if (PMU->isPekeyLongPressIrq()) {
        ESP_LOGD(TAG, "isPekeyLongPress");
    }
    if (PMU->isBatChargeDoneIrq()) {
        ESP_LOGD(TAG, "isBatChargeDone");
    }
    if (PMU->isBatChargeStartIrq()) {
        ESP_LOGD(TAG, "isBatChargeStart");
    }
    // Clear PMU Interrupt Status Register
    PMU->clearIrqStatus();
}
#endif
