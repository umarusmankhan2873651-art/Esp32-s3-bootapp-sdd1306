#include "flasher.h"
#include "config.h"
#include "hal.h"
#include "ui.h"
#include <SD.h>
#include <esp_ota_ops.h>
#include <esp_partition.h>

static const esp_partition_t* slot() {
  return esp_partition_find_first(ESP_PARTITION_TYPE_APP, SLOT_SUBTYPE, NULL);
}

bool flasher_slot_size(uint32_t* out) {
  const esp_partition_t* s = slot();
  if (!s) return false;
  *out = s->size;
  return true;
}

bool flasher_write(const Manifest* m) {
  const esp_partition_t* s = slot();
  if (!s) { ui_message("no ota_0 slot"); return false; }
  if (m->size > s->size) { ui_message("image too big"); return false; }

  char path[MF_PATH_LEN + 48];
  snprintf(path, sizeof(path), "%s/%s", m->dir, m->app);
  File f = SD.open(path, FILE_READ);
  if (!f) { ui_message("open failed"); return false; }

  ui_progress("erasing", 0);
  if (esp_partition_erase_range(s, 0, s->size) != ESP_OK) {
    f.close(); ui_message("erase failed"); return false;
  }

  static uint8_t buf[4096] __attribute__((aligned(4)));
  uint32_t off = 0;
  while (f.available()) {
    size_t n = f.read(buf, sizeof(buf));
    if (n == 0) break;
    size_t aligned = (n + 3) & ~3u;
    for (size_t i = n; i < aligned; i++) buf[i] = 0xFF;
    if (esp_partition_write(s, off, buf, aligned) != ESP_OK) {
      f.close(); ui_message("write failed"); return false;
    }
    off += aligned;
    int pct = (int)((uint64_t)off * 100 / m->size);
    ui_progress("flashing", pct > 100 ? 100 : pct);
  }
  f.close();

  if (off < m->size) { ui_message("short write"); return false; }
  ui_progress("done", 100);
  return true;
}

void flasher_boot_slot() {
  const esp_partition_t* s = slot();
  if (!s) return;
  if (esp_ota_set_boot_partition(s) != ESP_OK) {
    ui_message("set boot failed");
    return;
  }
  ui_toast("booting...", 700);
  esp_restart();
}