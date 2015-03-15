#include "adb.h"
#include "adb_private.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

// --------------
// Timer/Counter0

#define MICROSECONDS_PRESCALED(microseconds, prescale) \
  ((uint8_t)(F_CPU / prescale * microseconds / 1000000))

// TCCR0B CS00:CS02 prescale values
#define PRESCALE_1 0x1
#define PRESCALE_8 0x2
#define PRESCALE_64 0x3
#define PRESCALE_256 0x4
#define PRESCALE_1024 0x5

// Public

void adb_command_data_16(uint16_t data) {
  adb_data_mode_output();
  _delay_us(150); // Tlt (stop-to-start time)
  adb_send_high(); // start-bit
  adb_send_byte((uint8_t)(data >> 8));
  adb_send_byte((uint8_t)data);
  adb_send_low(); // stop-bit
  adb_data_mode_input();
}

/**
 * Read an expected 16-bit response from ADB device.
 * Assumes ADB_PORT's DDR is set to input.
 */
void adb_receive_16(struct adb_response_16 * response) {
  uint8_t low_times[18];
  for (int i = 0; i < 18; i++) {
    if (adb_wait_for_low_with_timeout() != 0) {
      response->timed_out = 1;
      return;
    }
    low_times[i] = adb_measure_time_until_high();
  }

  uint16_t value = 0;
  for (int i = 1; i < 17; i++) { // ignore start/stop bit
    uint8_t lt = low_times[i];
    value <<= 1;
    if (lt > 25 && lt < 45) value |= 1;
  }

  response->timed_out = 0;
  response->a = (uint8_t)(value >> 8);
  response->b = (uint8_t)value;
}

void adb_reset() {
  _delay_ms(100); // stabilize on power-up
  adb_data_mode_output();
  ADB_LOW();
  _delay_ms(3);
  ADB_HIGH();
  adb_data_mode_input();
  _delay_ms(200); // give time to reset
}

void adb_send_command(struct adb_cmd * cmd) {
  adb_data_mode_output();
  ADB_LOW();
  _delay_us(800); // Attention signal
  ADB_HIGH();
  _delay_us(70); // Sync signal
  adb_command_packet(cmd);
  adb_data_mode_input();
}


// Private

static uint8_t adb_cmd_to_byte(struct adb_cmd * cmd) {
  return (cmd->address << 4) | (cmd->command << 2) | (cmd->reg);
}

static void adb_command_packet(struct adb_cmd * cmd) {
  adb_send_byte(adb_cmd_to_byte(cmd));
  adb_send_low(); // stop-bit
}

static void adb_data_mode_input() {
  ADB_DDR &= ~ADB_DATA_MASK;
  ADB_PORT &= ~ADB_DATA_MASK; // hi-Z input
}

static void adb_data_mode_output() {
  ADB_HIGH(); // output high to match pull-up resistor
  ADB_DDR |= ADB_DATA_MASK;
}

static uint8_t adb_measure_time_until_high() {
  TCCR0A = 0x00; // defaults
  TCCR0B = 0x00; // defaults, stop timer clock source
  TCNT0 = 0x00; // reset counter
  TCCR0B |= (PRESCALE_8 << CS00); // start clock
  while (ADB_IS_LOW); // TODO: timeout?
  TCCR0B = 0x00; // stop clock
  return (uint8_t)((uint16_t)TCNT0 * 8 / (F_CPU / 1000000));
}

static void adb_send_byte(uint8_t byte) {
  for (int i = 0; i < 8; i++) {
    if (byte & _BV(7)) {
      adb_send_high();
    } else {
      adb_send_low();
    }
    byte <<= 1;
  }
}

static void adb_send_high() {
  ADB_LOW();
  _delay_us(35);
  ADB_HIGH();
  _delay_us(65);
}

static void adb_send_low() {
  ADB_LOW();
  _delay_us(65);
  ADB_HIGH();
  _delay_us(35);
}

/**
 * Returns 0 on low signal, or 2 on timeout.
 */
static uint8_t adb_wait_for_low_with_timeout() {
  TCCR0A = 0x00; // defaults
  TCCR0B = 0x00; // defaults, stop timer clock source
  TCNT0 = 0x00; // reset counter
  TIFR0 |= _BV(OCF0A); // clear flag counterintuitively by writing 1
  OCR0A = MICROSECONDS_PRESCALED(500, 64); // set compare value
  TCCR0B |= (PRESCALE_64 << CS00); // start clock
  while (1) {
    if (ADB_IS_LOW) return 0;
    if (TIFR0 & _BV(OCF0A)) return 2;
  }
}
