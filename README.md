# Arduino library for LoRa boards

## Features 

- Quick kickstart your [LoRa](https://lora-alliance.org) projects
- Supports [PlatformIO](https://platformio.org) and [pioarduino](https://github.com/pioarduino/platform-espressif32) as platforms 
- Supports multiple firmwares builds with github actions
- Example codes

## Prerequisites

- [VSCode](https://code.visualstudio.com) IDE

- [PlatformIO](https://platformio.org/install/ide?install=vscode) or [pioarduino](https://platformio.org/install/ide?install=vscode) extension for [VSCode](https://code.visualstudio.com/) IDE


## Get started

1. Use an ESP32 project template like
```sh
git clone https://github.com/mcuw/ESP32-ghbuild-template.git YOUR-APP
```
replace `YOUR-APP` with your custom application name

2. add this library to your Arduino project, search for `MCUW LoRa boards`

3. copy the config template to lib/config/config.h

template https://raw.githubusercontent.com/mcuw/LoRaBoards/refs/heads/main/lib/config/config.h
```sh
mkdir lib/config
cd lib/config
wget https://raw.githubusercontent.com/mcuw/LoRaBoards/refs/heads/main/lib/config/config.h
```

4. ensure that the config can be find as includes
```ini
[env]
build-flags = -I lib/config
```

5. Enable LoRa boards features

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
