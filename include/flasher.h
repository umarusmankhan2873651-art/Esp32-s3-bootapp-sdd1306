#pragma once
#include "manifest.h"
bool flasher_slot_size(uint32_t* out);
bool flasher_write(const Manifest* m);   // returns true on success (no restart)
void flasher_boot_slot();