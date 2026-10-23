#include "slayerx_cfg.h"

SlayerxCfg g_cfg;

static const esp_partition_t* cfg_part() {
  return esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                  ESP_PARTITION_SUBTYPE_DATA_NVS, "cfg");
}

void cfg_defaults() {
  memset(&g_cfg, 0, sizeof(g_cfg));
  g_cfg.magic   = CFG_MAGIC;
  g_cfg.version = CFG_VERSION;
  const uint8_t b[6] = { BTN_UP, BTN_DOWN, BTN_SELECT, BTN_BACK, BTN_LEFT, BTN_RIGHT };
  memcpy(g_cfg.btn_pins, b, 6);
  g_cfg.i2c_sda = PIN_I2C_SDA;  g_cfg.i2c_scl = PIN_I2C_SCL;  g_cfg.oled_addr = OLED_ADDR;
  g_cfg.ir_tx   = PIN_IR_TX;    g_cfg.ir_rx   = PIN_IR_RX;
  g_cfg.nrf_ce  = PIN_NRF_CE;   g_cfg.nrf_csn = PIN_NRF_CSN;
  g_cfg.spi_sck = PIN_SPI_SCK;  g_cfg.spi_miso = PIN_SPI_MISO; g_cfg.spi_mosi = PIN_SPI_MOSI;
  g_cfg.cc1101_cs = PIN_CC_CS;  g_cfg.cc1101_gdo0 = PIN_CC_GDO0; g_cfg.cc1101_gdo2 = PIN_CC_GDO2;
  g_cfg.sd_cs = PIN_SD_CS; g_cfg.sd_sck = PIN_SD_SCK; g_cfg.sd_miso = PIN_SD_MISO; g_cfg.sd_mosi = PIN_SD_MOSI;
  g_cfg.brightness = 255;
  g_cfg.flags = 0;
}

bool cfg_load() {
  const esp_partition_t* p = cfg_part();
  if (!p) return false;
  if (esp_partition_read(p, 0, &g_cfg, sizeof(g_cfg)) != ESP_OK) return false;
  if (g_cfg.magic != CFG_MAGIC || g_cfg.version != CFG_VERSION) return false;
  return true;
}

bool cfg_save() {
  const esp_partition_t* p = cfg_part();
  if (!p) return false;
  esp_partition_erase_range(p, 0, p->size);
  return esp_partition_write(p, 0, &g_cfg, sizeof(g_cfg)) == ESP_OK;
}

void cfg_reset() { cfg_defaults(); cfg_save(); }