#include "adb.h"
#include <avr/io.h>
#include <util/delay.h>

void setup();
void loop();

int main() {
  setup();
  while (1) loop();
}

void setup() {
  adb_reset();
  _delay_ms(1); // arbitrary
  struct adb_cmd cmd = {
    .address = ADB_KB_ADDRESS,
    .command = ADB_COMMAND_TALK,
    .reg = ADB_REGISTER_INFO
  };
  adb_send_command(cmd);
}

void loop() {
}
