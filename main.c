#include "adb.h"
#include "adb_keyboard.h"
#include "aek2_keys.h"
#include "keybuffer.h"
#include "usbdrv/usbdrv.h"
#include "keymap.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

static void setup();
static void setup_keybuffer();
static void setup_adb();
static void setup_usb();
static void loop();
static void poll_adb();
static void poll_usb();
static void handle_keyboard_transition(uint8_t t);

struct keyboard_report_t {
	uint8_t modifier;
	uint8_t reserved;
	uint8_t keycode[6];
};

static struct keybuffer_t keybuffer;
static struct keyboard_report_t keyboard_report;
static uchar idleRate; // repeat rate for keyboards

int main() {
  setup();
  while (1) loop();
}

static void setup() {
  setup_keybuffer();
  setup_usb();
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

static void setup_usb() {
  keyboard_report.modifier = 0;
  usbInit();
  usbDeviceDisconnect();
  _delay_ms(500);
  usbDeviceConnect();
}

static void loop() {
  poll_adb();
  poll_usb();
}

static void poll_adb() {
  struct adb_response_16 response;
  adb_keyboard_poll(&response);
  if (!response.timed_out) {
    handle_keyboard_transition(response.a);
    handle_keyboard_transition(response.b);
    if (usbInterruptIsReady()) {
      usbSetInterrupt((void *)&keyboard_report, sizeof(keyboard_report));
    }
  }
}

static void poll_usb() {
  usbPoll(); // delegate to V-USB
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

// V-USB

// USB HID descriptor
// From http://codeandlife.com/2012/06/18/usb-hid-keyboard-with-v-usb/ which in
// turn attributes http://www.frank-zhao.com/cache/usbbusinesscard_details.php
PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)(Key Codes)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)(224)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)(231)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs) ; Modifier byte
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs) ; Reserved byte
    0x95, 0x05,                    //   REPORT_COUNT (5)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x05, 0x08,                    //   USAGE_PAGE (LEDs)
    0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
    0x29, 0x05,                    //   USAGE_MAXIMUM (Kana)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs) ; LED report
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x03,                    //   REPORT_SIZE (3)
    0x91, 0x03,                    //   OUTPUT (Cnst,Var,Abs) ; LED report padding
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)(Key Codes)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))(0)
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)(101)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0                           // END_COLLECTION
};

// Thanks to http://codeandlife.com/2012/06/18/usb-hid-keyboard-with-v-usb/
usbMsgLen_t usbFunctionSetup(uchar data[8]) {
  usbRequest_t *rq = (void *)data;
  if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
    switch(rq->bRequest) {

      case USBRQ_HID_GET_REPORT:
        usbMsgPtr = (void *)&keyboard_report;
        return sizeof(keyboard_report);

      case USBRQ_HID_SET_REPORT: // if wLength == 1, should be LED state
        return (rq->wLength.word == 1) ? USB_NO_MSG : 0;

      case USBRQ_HID_GET_IDLE: // send idle rate to PC as required by spec
        usbMsgPtr = &idleRate;
        return 1;

      case USBRQ_HID_SET_IDLE: // save idle rate as required by spec
        idleRate = rq->wValue.bytes[1];
        return 0;
    }
  }
  return 0;
}

usbMsgLen_t usbFunctionWrite(uint8_t * data, uchar len) {
  return 1;
}
