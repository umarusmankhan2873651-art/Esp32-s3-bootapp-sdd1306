#pragma once
#include <Arduino.h>
#include <esp_partition.h>
#include "slayerx_pins.h"

#define CFG_MAGIC   0x534C5858u
#define CFG_VERSION 1

struct __attribute__((packed)) SlayerxCfg {
  uint32_t magic;
  uint16_t version;
  uint8_t  btn_pins[6];      // UP,DOWN,SELECT,BACK,LEFT,RIGHT
  uint8_t  i2c_sda, i2c_scl, oled_addr;
  uint8_t  ir_tx, ir_rx;
  uint8_t  nrf_ce, nrf_csn, spi_sck, spi_miso, spi_mosi;
  uint8_t  cc1101_cs, cc1101_gdo0, cc1101_gdo2;
  uint8_t  sd_cs, sd_sck, sd_miso, sd_mosi;
  uint8_t  brightness, flags;
};

extern SlayerxCfg g_cfg;

void cfg_defaults();
bool cfg_load();
bool cfg_save();
void cfg_reset();