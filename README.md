# SlayerX Loader

Multi-firmware boot manager for ESP32-S3 (N16R8) + SSD1306 OLED + 6 buttons.

## How it works
Boots to a menu. Pick a firmware from SD -> it is written to the `ota_0`
partition -> reboots into it. The firmware clears `otadata` at startup, so the
next hardware reset always returns to SlayerX.

## Flash the loader
    pio run -t upload

## Prepare a firmware
    python tools/pack_firmware.py <build_dir> --name Bruce --version 1.9.0

Then copy `dist/bruce/` to `/slayerx/firmware/bruce/` on the SD card.

## Make a firmware returnable
Add to its `setup()`, first line:

    #include "slayerx_boot.h"
    slayerx_return_to_loader();

## SD layout
    /slayerx/firmware/<name>/manifest.json
    /slayerx/firmware/<name>/app.bin
    /slayerx/firmware/<name>/assets/       (optional)

## Pin map
See `include/slayerx_pins.h`. Change pins once, everything follows.