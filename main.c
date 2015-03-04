#include "adb.h"
#include "adb_keyboard.h"
#include "avr-uart/uart.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

void setup();
void loop();
void print_keyboard_transition(uint8_t t);
void initialize_uart();
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
  printf(
    "%-4s: %02X (%d)\n",
    (t & _BV(7)) ? "  up" : "down",
    t & 0b01111111,
    t & 0b01111111
  );
}

void initialize_uart() {
  stdout = &uart0_stdout;
  sei();
  uart0_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
}

int uart0_putchar_printf(char c, FILE *stream) {
  if (c == '\n') uart0_putc('\r');
  uart0_putc(c);
  return 0;
}
