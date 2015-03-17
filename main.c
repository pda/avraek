#include "adb.h"
#include "adb_keyboard.h"
#include "aek2_keys.h"
#include "keybuffer.h"
#include "usb_keyboard.h"
#include "keymap.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

static void setup();
static void setup_keybuffer();
static void setup_adb();
static void loop();
static void poll_adb();
static void handle_keyboard_transition(uint8_t t);

int main() {
  setup();
  while (1) loop();
}

static void setup() {
  setup_keybuffer();
  usb_keyboard_init();
  setup_adb();
  sei();
}

static void setup_keybuffer() {
  keybuffer_init(&keybuffer, &(keyboard_report.keycode[0]));
}

static void setup_adb() {
  adb_reset();
  adb_keyboard_initialize();
}

static void loop() {
  poll_adb();
  usb_keyboard_poll();
}

static void poll_adb() {
  struct adb_response_16 response;
  adb_keyboard_poll(&response);
  if (!response.timed_out) {
    handle_keyboard_transition(response.a);
    handle_keyboard_transition(response.b);
    usb_keyboard_interrupt_with_keyboard_report();
  }
}

static void handle_keyboard_transition(uint8_t t) {
  if (t == 0xFF) return;
  uint8_t isUp = t >> 7;
  uint8_t key_adb = t & 0x7F;
  uint8_t key_usb = aek2_keymap[key_adb];
  if (isUp) {
    switch (key_adb) {
      case ADB_KEY_CTRL:
        keyboard_report.modifier &= ~_BV(0);
        break;
      case ADB_KEY_SHIFT:
        keyboard_report.modifier &= ~_BV(1);
        break;
      case ADB_KEY_OPTION:
        keyboard_report.modifier &= ~_BV(2);
        break;
      case ADB_KEY_CMD:
        keyboard_report.modifier &= ~_BV(3);
        break;
      default:
        keybuffer_up(&keybuffer, key_usb);
        break;
    }
  } else {
    switch (key_adb) {
      case ADB_KEY_CTRL:
        keyboard_report.modifier |= _BV(0);
        break;
      case ADB_KEY_SHIFT:
        keyboard_report.modifier |= _BV(1);
        break;
      case ADB_KEY_OPTION:
        keyboard_report.modifier |= _BV(2);
        break;
      case ADB_KEY_CMD:
        keyboard_report.modifier |= _BV(3);
        break;
      default:
        keybuffer_down(&keybuffer, key_usb);
        break;
    }
  }
}
