#pragma once

#include "keybuffer.h"

#include <stdint.h>

struct keybuffer_t keybuffer;
struct keyboard_report_t keyboard_report;

struct keyboard_report_t {
	uint8_t modifier;
	uint8_t reserved;
	uint8_t keycode[6];
};

void usb_keyboard_init();
void usb_keyboard_poll();
void usb_keyboard_interrupt_with_keyboard_report();
