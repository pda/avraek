#pragma once

#include <stdint.h>

void adb_reset();
void adb_keyboard_initialize();
uint16_t adb_keyboard_poll();
