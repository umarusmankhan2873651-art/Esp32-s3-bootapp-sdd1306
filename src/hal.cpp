#include "hal.h"
#include <SD.h>          // <-- ADD THIS
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

static SPIClass sdBus(HSPI);
static bool sdmounted = false;
static uint32_t btnLast[B_COUNT] = {0};

void hal_init() {
  Serial.begin(115200);
  delay(150);
  if (!cfg_load()) { cfg_defaults(); cfg_save(); }

  for (int i = 0; i < B_COUNT; i++) pinMode(g_cfg.btn_pins[i], INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  Wire.begin(g_cfg.i2c_sda, g_cfg.i2c_scl);
  display.begin();
  display.setContrast(g_cfg.brightness);
  hal_font_small();
  display.clearBuffer();
  display.sendBuffer();
}

void hal_clear() { display.clearBuffer(); }
void hal_flush() { display.sendBuffer(); }
void hal_font_small() { display.setFont(u8g2_font_6x10_tr); }
void hal_font_big()   { display.setFont(u8g2_font_ncenB14_tr); }
void hal_text(int x, int y, const char* s) { display.drawStr(x, y, s); }
void hal_text_center(int y, const char* s) {
  int w = display.getStrWidth(s);
  display.drawStr((OLED_W - w) / 2, y, s);
}

bool btn_pressed(Btn b) {
  if (digitalRead(g_cfg.btn_pins[b]) == LOW) {
    uint32_t now = millis();
    if (now - btnLast[b] > DEBOUNCE_MS) { btnLast[b] = now; return true; }
  }
  return false;
}
bool btn_held(Btn b) { return digitalRead(g_cfg.btn_pins[b]) == LOW; }
void btn_wait_release(Btn b) { while (digitalRead(g_cfg.btn_pins[b]) == LOW) delay(1); }

void led_set(uint8_t r, uint8_t g, uint8_t b) { (void)r; (void)g; (void)b; }

void buzz(uint16_t ms) {
  digitalWrite(PIN_BUZZER, HIGH);
  delay(ms);
  digitalWrite(PIN_BUZZER, LOW);
}

SPIClass& sd_spi() { return sdBus; }

bool sd_begin() {
  if (sdmounted) return true;
  sdBus.begin(g_cfg.sd_sck, g_cfg.sd_miso, g_cfg.sd_mosi, g_cfg.sd_cs);
  sdmounted = SD.begin(g_cfg.sd_cs, sdBus, 20000000);
  return sdmounted;
}
bool sd_ok() { return sdmounted; }
