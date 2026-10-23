#pragma once
#include <Arduino.h>

typedef void (*UiAction)();

struct UiItem { const char* label; UiAction action; const char* badge; };

void ui_splash();
void ui_message(const char* l1, const char* l2 = nullptr, uint16_t ms = 1400);
void ui_toast(const char* msg, uint16_t ms = 900);
bool ui_confirm(const char* q);
void ui_progress(const char* label, int percent);
bool ui_menu(const char* title, const UiItem* items, uint8_t count);
int  ui_pick(const char* title, const char* const* opts, int n);