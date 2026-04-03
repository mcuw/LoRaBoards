# Arduino library for LoRa boards

<video src="https://github.com/user-attachments/assets/22d95d18-f688-4b5a-a8a5-95e3825e6179" alt="LoRa demo with three boards"></video>

## Features 

- Quick kickstart your [LoRa](https://lora-alliance.org) projects
- Supports [PlatformIO](https://platformio.org) and [pioarduino](https://github.com/pioarduino/platform-espressif32) as platforms 
- Example [codes](examples/)

## Prerequisites

- [VSCode](https://code.visualstudio.com) IDE

- [PlatformIO](https://platformio.org/install/ide?install=vscode) or [pioarduino](https://platformio.org/install/ide?install=vscode) extension as [VSCode extensions](https://marketplace.visualstudio.com/VSCode)


## Get started

1. Use an ESP32 project template like
```sh
git clone https://github.com/mcuw/ESP32-ghbuild-template.git YOUR-APP
```
replace `YOUR-APP` with your custom application name

2. Add this library to your Arduino project, search for `MCUW LoRa boards`

3. Copy the config template to lib/config/config.h

template https://raw.githubusercontent.com/mcuw/LoRaBoards/refs/heads/main/lib/config/config.h
```sh
mkdir -p lib/config
cd lib/config
wget https://raw.githubusercontent.com/mcuw/LoRaBoards/refs/heads/main/lib/config/config.h
```

4. Ensure that the config can be find as includes
```ini
[env]
build-flags = -I lib/config
```

5. Enable LoRa boards features and add libraries

add libraries
```ini
lib_deps =
  ; RadioLib - LoRa library
  ; releases https://github.com/jgromes/RadioLib/releases
  jgromes/RadioLib @ ^7.6.0
  ; button handling library, used e.g. in TTGO T-Beam
  mathertel/OneButton @ ^2.6.2
  ; XPowersLib - power management series library for ESP32, including PSRAM power management
  lewisxhe/XPowersLib @ ^0.3.3
  ; OLED driver for SSD1306 displays, used e.g. in TTGO Lora32 v1.6.1
  thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 displays @ ^4.6.2
```

add `build_flags` in platformio.ini
```ini
build_flags =
    -D HAS_BUTTON
    -D HAS_PMU
    -D HAS_GPS
    -D USE_SX1262
```

## Pinouts and config

### ESP32 LilyGo Series

Manufacture examples for LilyGo boards. T-Beam, T3-S3, LORA32 and more, see https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/blob/master/examples/TBeamFactory/utilities.h
