#ifdef HAS_NTC
//NCP18XH103F03RB: https://item.szlcsc.com/14214.html
// #define NTC_PIN 14 // NTC connection pins
#define SERIES_RESISTOR 10000 // Series resistance value (10kΩ)
#define B_COEFFICIENT 3950 // B value, set according to the NTC specification
#define ROOM_TEMP 298.15 // 25°C absolute temperature (K)
#define ROOM_TEMP_RESISTANCE 10000 // Resistance of NTC at 25°C (10kΩ)

#define CALIBRATE_ONE(cali_clk) calibrate_one(cali_clk, #cali_clk)

float getTempForNTC()
{
    static float temperature = 0.0f;
#ifdef NTC_PIN
    static uint32_t check_temperature = 0;
    if (millis() > check_temperature) {
        float voltage = analogReadMilliVolts(NTC_PIN) / 1000.0;
        float resistance = SERIES_RESISTOR * ((3.3 / voltage) - 1); // Calculate the resistance of NTC

        // Calculate temperature using the Steinhart-Hart equation
        temperature = (1.0 / (log(resistance / ROOM_TEMP_RESISTANCE) / B_COEFFICIENT + 1.0 / ROOM_TEMP)) - 273.15;

        ESP_LOGD(TAG, "Temperature: %f °C", temperature);

        check_temperature  = millis() + 1000;
    }
#endif
    return temperature;
}

static uint32_t calibrate_one(rtc_cal_sel_t cal_clk, const char *name)
{
    const uint32_t cal_count = 1000;
    const float factor = (1 << 19) * 1000.0f;
    uint32_t cali_val;
    for (int i = 0; i < 5; ++i) {
        cali_val = rtc_clk_cal(cal_clk, cal_count);
    }
    return cali_val;
}

static void enable_slow_clock()
{
    rtc_clk_32k_enable(true);
    CALIBRATE_ONE(RTC_CAL_RTC_MUX);
    uint32_t cal_32k = CALIBRATE_ONE(RTC_CAL_32K_XTAL);
    if (cal_32k == 0) {
        Serial.printf("32K XTAL OSC has not started up");
    } else {
        rtc_clk_slow_freq_set(RTC_SLOW_FREQ_32K_XTAL);
        ESP_LOGD(TAG, "Switching RTC Source to 32.768Khz succeeded, using 32K XTAL");
        CALIBRATE_ONE(RTC_CAL_RTC_MUX);
        CALIBRATE_ONE(RTC_CAL_32K_XTAL);
    }
    CALIBRATE_ONE(RTC_CAL_RTC_MUX);
    CALIBRATE_ONE(RTC_CAL_32K_XTAL);
    if (rtc_clk_slow_freq_get() != RTC_SLOW_FREQ_32K_XTAL) {
        ESP_LOGD(TAG, "Warning: Failed to set rtc clk to 32.768Khz !!! "); return;
    }
    deviceOnline |= OSC32768_ONLINE;
}

#endif /* HAS_NTC */