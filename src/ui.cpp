#include "ui.h"
#include "hal.h"
#include "config.h"

static void draw_title(const char* t) {
  display.drawBox(0, 0, OLED_W, 12);
  display.setDrawColor(0);
  hal_font_small();
  hal_text(2, 10, t);
  display.setDrawColor(1);
}

void ui_splash() {
  hal_clear();
  // hexagon frame
  display.drawLine(38, 2, 90, 2);
  display.drawLine(90, 2, 106, 32);
  display.drawLine(106, 32, 90, 62);
  display.drawLine(90, 62, 38, 62);
  display.drawLine(38, 62, 22, 32);
  display.drawLine(22, 32, 38, 2);
  // chevron blades
  display.drawLine(52, 22, 76, 32);
  display.drawLine(76, 32, 52, 42);
  display.drawLine(60, 26, 84, 36);
  display.drawLine(84, 36, 60, 46);
  hal_font_big();
  hal_text_center(58, "SLAYERX");
  hal_flush();
  delay(1200);
}

void ui_message(const char* l1, const char* l2, uint16_t ms) {
  hal_clear();
  hal_font_small();
  hal_text_center(30, l1);
  if (l2) hal_text_center(46, l2);
  hal_flush();
  delay(ms);
}

void ui_toast(const char* msg, uint16_t ms) {
  hal_clear();
  hal_font_small();
  hal_text_center(34, msg);
  hal_flush();
  delay(ms);
}

bool ui_confirm(const char* q) {
  const char* o[] = { "Yes", "No" };
  return ui_pick(q, o, 2) == 0;
}

void ui_progress(const char* label, int percent) {
  hal_clear();
  hal_font_small();
  hal_text(2, 12, label);
  display.drawFrame(2, 30, 124, 12);
  int w = (percent * 120) / 100;
  if (w > 0) display.drawBox(3, 31, w, 10);
  char b[8];
  snprintf(b, sizeof(b), "%d%%", percent);
  hal_text(58, 56, b);
  hal_flush();
}

bool ui_menu(const char* title, const UiItem* items, uint8_t count) {
  int sel = 0;
  while (true) {
    hal_clear();
    draw_title(title);
    int first = (sel >= 4) ? sel - 3 : 0;
    for (int i = 0; i < 4 && (first + i) < count; i++) {
      int idx = first + i;
      int y = 24 + i * 12;
      if (idx == sel) { display.drawBox(0, y - 10, OLED_W, 12); display.setDrawColor(0); }
      hal_text(4, y, items[idx].label);
      if (items[idx].badge) {
        int w = display.getStrWidth(items[idx].badge);
        hal_text(OLED_W - w - 2, y, items[idx].badge);
      }
      display.setDrawColor(1);
    }
    if (count > 4) {
      char b[16];
      snprintf(b, sizeof(b), "%d/%d", sel + 1, count);
      hal_text(2, 63, b);
    }
    hal_flush();

    if (btn_pressed(B_DOWN)) sel = (sel + 1) % count;
    if (btn_pressed(B_UP))   sel = (sel - 1 + count) % count;
    if (btn_pressed(B_SELECT)) {
      btn_wait_release(B_SELECT);
      if (items[sel].action) items[sel].action();
    }
    if (btn_pressed(B_BACK)) { btn_wait_release(B_BACK); return true; }
    delay(10);
  }
}

int ui_pick(const char* title, const char* const* opts, int n) {
  int sel = 0;
  while (true) {
    hal_clear();
    draw_title(title);
    for (int i = 0; i < n && i < 4; i++) {
      int y = 24 + i * 12;
      if (i == sel) { display.drawBox(0, y - 10, OLED_W, 12); display.setDrawColor(0); }
      hal_text(4, y, opts[i]);
      display.setDrawColor(1);
    }
    hal_flush();
    if (btn_pressed(B_DOWN)) sel = (sel + 1) % n;
    if (btn_pressed(B_UP))   sel = (sel - 1 + n) % n;
    if (btn_pressed(B_SELECT)) { btn_wait_release(B_SELECT); return sel; }
    if (btn_pressed(B_BACK))   { btn_wait_release(B_BACK); return -1; }
    delay(10);
  }
}