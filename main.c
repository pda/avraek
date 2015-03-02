#include "adb.h"
#include "avr-uart/uart.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void setup();
void loop();
void initialize_uart();
void initialize_adb();
void initialize_adb_keyboard();
int uart0_putchar_printf(char c, FILE *stream);

static FILE uart0_stdout = FDEV_SETUP_STREAM(
  uart0_putchar_printf, NULL, _FDEV_SETUP_WRITE
);

int main() {
  setup();
  while (1) loop();
}

void setup() {
  initialize_uart();
  printf("\n\n");
  printf("Initializing ADB\n");
  initialize_adb();
  printf("Initializing keyboard\n");
  initialize_adb_keyboard();
  printf("Done\n");
}

void loop() {
  _delay_ms(200);
}

void initialize_uart() {
  stdout = &uart0_stdout;
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
  uint16_t response = adb_read16();
  printf("response: 0x%04X\r\n", response);
  adb_keyboard_animate_leds();
}

int uart0_putchar_printf(char c, FILE *stream) {
  if (c == '\n') uart0_putc('\r');
  uart0_putc(c);
  return 0;
}
