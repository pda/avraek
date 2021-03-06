#include "adb.h"
#include "adb_keyboard.h"
#include "aek2_keys.h"
#include "keybuffer.h"
#include "usb_keyboard.h"
#include "keymap.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

static void poll_adb();
static void handle_keyboard_transition(uint8_t t);

int main() {
  usb_keyboard_init();
  adb_keyboard_initialize();
  sei();
  while (1) {
    poll_adb();
    usb_keyboard_poll();
  }
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
