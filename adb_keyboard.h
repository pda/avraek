#pragma once

#include <stdint.h>

// Functions

void adb_keyboard_initialize();
void adb_keyboard_poll(struct adb_response_16 *);
