#include <Arduino.h>
#include "hal.h"
#include "ui.h"
#include "config.h"
#include "sdbrowse.h"
#include "flasher.h"
#include "slayerx_cfg.h"
#include <SD.h>
// ---- SlayerX shim ----
#include <esp_ota_ops.h>
#include <esp_partition.h>
static inline void slayerx_return_to_loader() {
  const esp_partition_t* p = esp_partition_find_first(
      ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
  if (p) esp_ota_set_boot_partition(p);
}

static void act_sd() {
  if (!sd_ok()) { ui_message("no SD card"); return; }
  browse_path(PATH_ROOT);
}

static void act_fw() {
  if (!sd_ok()) { ui_message("no SD card"); return; }
  browse_firmware_dir();
}

static void act_cfg() {
  const char* o[] = { "Brightness +", "Brightness -", "Reset pins", "Back" };
  int c = ui_pick("Config", o, 4);
  if (c == 0) { g_cfg.brightness = (g_cfg.brightness > 235) ? 255 : g_cfg.brightness + 20; display.setContrast(g_cfg.brightness); cfg_save(); }
  if (c == 1) { g_cfg.brightness = (g_cfg.brightness < 40) ? 20 : g_cfg.brightness - 20; display.setContrast(g_cfg.brightness); cfg_save(); }
  if (c == 2) { if (ui_confirm("reset pin map?")) { cfg_reset(); ui_message("saved, reboot"); } }
}

static void act_about() {
  hal_clear();
  hal_font_small();
  hal_text(2, 12, FW_NAME);
  hal_text(2, 26, "v" FW_VERSION);
  hal_text(2, 40, "ESP32-S3 / 16MB");
  uint32_t sz = 0;
  flasher_slot_size(&sz);
  char b[32];
  snprintf(b, sizeof(b), "slot: %lu KB", (unsigned long)(sz / 1024));
  hal_text(2, 54, b);
  hal_flush();
  delay(2500);
}

static const UiItem mainItems[] = {
  { "SD Card",  act_sd,    nullptr },
  { "Firmware", act_fw,    "boot"  },
  { "Config",   act_cfg,   nullptr },
  { "About",    act_about, nullptr },
};

void setup() {
  slayerx_return_to_loader();   // <-- added
  // ... whatever was there befor
  hal_init();
  ui_splash();
  if (sd_ok()) SD.mkdir(PATH_ROOT);
  led_set(0, 0, 0);
}

void loop() {
  ui_menu("SLAYERX", mainItems, 4);
}