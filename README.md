# ESP-Google-IoT

This framework enables Google Cloud IoT Core connectivity with ESP32 based platforms using [Google Cloud IoT Device SDK](https://github.com/GoogleCloudPlatform/iot-device-sdk-embedded-c/blob/master/README.md).


## Supported Hardware

- [ESP32-DevKitC](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html#esp32-devkitc-v4)
- [ESP-WROVER-KIT](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html#esp-wrover-kit-v4-1)
- [ESP32-PICO-KIT](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html#esp32-pico-kit-v4-1)

## Getting Started

- Please refer to https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html for setting ESP-IDF
  - ESP-IDF can be downloaded from https://github.com/espressif/esp-idf/
  - ESP-IDF v3.2 and above is recommended version
- Please refer to [example README](examples/smart_outlet/README.md) for setting up smart outlet use case which allows to control load connected to configurable GPIO on ESP32 using Google Cloud IoT Core
