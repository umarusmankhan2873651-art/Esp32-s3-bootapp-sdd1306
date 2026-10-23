#pragma once
#include <esp_ota_ops.h>
#include <esp_partition.h>

// Call as the FIRST line of setup() in any loadable firmware.
// Next hardware reset returns to the SlayerX loader.
static inline void slayerx_return_to_loader() {
  const esp_partition_t* p =
      esp_partition_find_first(ESP_PARTITION_TYPE_APP,
                               ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
  if (p) esp_ota_set_boot_partition(p);
}

struct slayerx_cfg_t {
  uint32_t magic; uint16_t version;
  uint8_t btn_pins[6];
  uint8_t i2c_sda, i2c_scl, oled_addr;
  uint8_t ir_tx, ir_rx;
  uint8_t nrf_ce, nrf_csn, spi_sck, spi_miso, spi_mosi;
  uint8_t cc1101_cs, cc1101_gdo0, cc1101_gdo2;
  uint8_t sd_cs, sd_sck, sd_miso, sd_mosi;
  uint8_t brightness, flags;
};

static inline bool slayerx_load_cfg(slayerx_cfg_t* out) {
  const esp_partition_t* p = esp_partition_find_first(
      ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "cfg");
  if (!p) return false;
  if (esp_partition_read(p, 0, out, sizeof(*out)) != ESP_OK) return false;
  return out->magic == 0x534C5858u;
}