#include "adb.h"
#include "avr-uart/uart.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

void setup();
void loop();
void initialize_uart();
void initialize_adb();
void initialize_adb_keyboard();

int main() {
  setup();
  while (1) loop();
}

void setup() {
  initialize_uart();
  uart0_puts("UART initialized\r\n");
  initialize_adb();
  uart0_puts("ADB initialized\r\n");
  initialize_adb_keyboard();
  uart0_puts("Keyboard initialized\r\n");
}

void loop() {
  _delay_ms(200);
}

void initialize_uart() {
  sei();
  uart0_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
}

void initialize_adb() {
  adb_reset();
}

void initialize_adb_keyboard() {
  struct adb_cmd cmd = {
    .address = ADB_KB_ADDRESS,
    .command = ADB_COMMAND_TALK,
    .reg = ADB_REGISTER_INFO
  };
  adb_send_command(cmd);
}
