#include "adb.h"
#include "adb_keyboard.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

void setup();
void loop();
void print_keyboard_transition(uint8_t t);
void initialize_adb_keyboard();

int main() {
  setup();
  while (1) loop();
}

void setup() {
  adb_reset();
  adb_keyboard_initialize();
}

void loop() {
  struct adb_response_16 response;
  adb_keyboard_poll(&response);
  if (!response.timed_out) {
    print_keyboard_transition(response.a);
    print_keyboard_transition(response.b);
  }
}

void print_keyboard_transition(uint8_t t) {
  if (t == 0xFF) return;
  // ...
}
