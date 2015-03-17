#include "adb.h"
#include "adb_keyboard.h"

#include <stdint.h>
#include <util/delay.h>

// Private headers

static void adb_keyboard_animate_leds();

// Public

void adb_keyboard_initialize() {
  struct adb_cmd cmd = {
    .address = ADB_KB_ADDRESS,
    .command = ADB_COMMAND_TALK,
    .reg = ADB_REGISTER_IDENTITY
  };
  struct adb_response_16 response;
  adb_send_command(&cmd);
  adb_receive_16(&response);
  adb_keyboard_animate_leds();
}

void adb_keyboard_poll(struct adb_response_16 * response) {
  struct adb_cmd cmd = {
    .address = ADB_KB_ADDRESS,
    .command = ADB_COMMAND_TALK,
    .reg = ADB_REGISTER_PRIMARY
  };
  adb_send_command(&cmd);
  adb_receive_16(response);
}

// Private

static void adb_keyboard_animate_leds() {
  struct adb_cmd listen = {
    .address = ADB_KB_ADDRESS,
    .command = ADB_COMMAND_LISTEN,
    .reg = ADB_REGISTER_2
  };
  uint16_t states[] = {
    //SCN: Scroll, Caps, Num
    0b001,
    0b011,
    0b111,
    0b111,
    0b110,
    0b100,
    0b000,
  };
  for (int i = 0; i < sizeof(states) / sizeof(uint16_t); i++) {
    adb_send_command(&listen);
    adb_command_data_16(~states[i]);
    _delay_ms(50);
  }
}
