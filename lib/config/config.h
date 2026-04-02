#pragma once

// activate customized board configuration which can be set with build flags in platformio.ini,
// e.g. -D USE_SX1262 or -D USE_SX1276 to select the LoRa module you are using

// Sub band
#if defined(USE_SX1262) || defined(USE_SX1276)
#define CONFIG_MAX_PACKET_LENGTH 255

#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ 868.0 // in MHz, must be within the frequency range of the module (e.g. 433.0 - 434.0, 868.0 or 915.0 MHz)
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW 62.5 // 7.8 - 500 kHz
#endif
#ifndef CONFIG_RADIO_SF
#define CONFIG_RADIO_SF 8 // 7-12
#endif
#ifndef CONFIG_RADIO_CR
#define CONFIG_RADIO_CR 8 // 5-8
#endif
#ifndef CONFIG_RADIO_CURRENT_LIMIT
#define CONFIG_RADIO_CURRENT_LIMIT 80 // in mA, accepted range is 45 - 240 mA, set value to 0 to disable overcurrent protection
#endif
#ifndef CONFIG_RADIO_PREAMBLE_LENGTH
#define CONFIG_RADIO_PREAMBLE_LENGTH 15 // in symbols, accepted range is 0 - 65535
#endif
#ifndef CONFIG_RADIO_CRC
#define CONFIG_RADIO_CRC false // enable or disable CRC
#endif

// #define ENABLE_BLE      //Enable ble function

#ifdef USE_SX1262
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER 22 // in dBm, accepted range is -17 - 22 dBm
#endif
#ifndef CONFIG_RADIO_SYNC_WORD
#define CONFIG_RADIO_SYNC_WORD 0x24 // LoRa sync word (0x34 for public networks, 0x24 for SX126x private networks)
#endif
#endif // USE_SX1262

#ifdef USE_SX1276
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER 20 // in dBm, accepted range is -17 - 20 dBm
#endif
#ifndef CONFIG_RADIO_SYNC_WORD
#define CONFIG_RADIO_SYNC_WORD 0x12 // LoRa sync word (0x34 for public networks, 0x12 for SX127x private networks)
#endif
#endif // USE_SX1276

// GPS
#ifndef CONFIG_RADIO_RESTART_ON_FAIL
#define CONFIG_RADIO_RESTART_ON_FAIL true
#endif

// higher prio than the lower prio tasks to ensure emergency button events are processed in time
#ifndef CONFIG_BUTTON_TASK_PRIORITY
#define CONFIG_BUTTON_TASK_PRIORITY 1
#endif
// run button task on core 1 to avoid conflicts with other tasks running on core 0, e.g. LoRa communication
#ifndef CONFIG_BUTTON_CORE
#define CONFIG_BUTTON_CORE ARDUINO_RUNNING_CORE
#endif
// button tick interval in milliseconds
#ifndef CONFIG_BUTTON_TICK_INTERVAL_MS
#define CONFIG_BUTTON_TICK_INTERVAL_MS 30
#endif

// LED task priority with lowest prio as default
// to ensure LED state is updated after processing other tasks, e.g. button events
#ifndef CONFIG_LED_TASK_PRIORITY
#define CONFIG_LED_TASK_PRIORITY 0
#endif
#ifndef CONFIG_LED_CORE
#define CONFIG_LED_CORE ARDUINO_RUNNING_CORE
#endif
#ifndef CONFIG_LED_TICK_INTERVAL_MS
#define CONFIG_LED_TICK_INTERVAL_MS 2
#endif

#endif // !defined(USE_SX1262) && !defined(USE_SX1276)

#ifdef HAS_DISPLAY_SSD1306
#ifndef DISPLAY_ADDR
// I2C address of the display, commonly 0x3C or 0x3D, check your display's datasheet or use an I2C scanner to find the correct address
#define DISPLAY_ADDR 0x3C
#endif // DISPLAY_ADDR
#endif // HAS_DISPLAY_SSD1306
