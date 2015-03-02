#include "adb.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void adb_reset() {
  _delay_ms(100); // stabilize on power-up
  adb_data_mode_output();
  ADB_LOW();
  _delay_ms(3);
  ADB_HIGH();
  adb_data_mode_input();
  _delay_ms(200); // give time to reset
}

void adb_data_mode_input() {
  ADB_DDR &= ~ADB_DATA_MASK;
  ADB_PORT &= ~ADB_DATA_MASK; // hi-Z input
}

void adb_data_mode_output() {
  ADB_HIGH(); // output high to match pull-up resistor
  ADB_DDR |= ADB_DATA_MASK;
}

void adb_send_command(struct adb_cmd cmd) {
  adb_data_mode_output();
  ADB_LOW();
  _delay_us(800); // Attention signal
  ADB_HIGH();
  _delay_us(70); // Sync signal
  adb_command_packet(cmd);
  adb_data_mode_input();
}

void adb_command_packet(struct adb_cmd cmd) {
  adb_send_byte(adb_cmd_to_byte(cmd));
  adb_send_low(); // stop-bit
}

void adb_send_byte(uint8_t byte) {
  for (int i = 0; i < 8; i++) {
    if (byte & (1 << 7)) {
      adb_send_high();
    } else {
      adb_send_low();
    }
    byte <<= 1;
  }
}

void adb_send_low() {
  ADB_LOW();
  _delay_us(65);
  ADB_HIGH();
  _delay_us(35);
}

void adb_send_high() {
  ADB_LOW();
  _delay_us(35);
  ADB_HIGH();
  _delay_us(65);
}

uint8_t adb_cmd_to_byte(struct adb_cmd cmd) {
  return (cmd.address << 4) | (cmd.command << 2) | (cmd.reg);
}

/**
 * Read an expected 16-bit response from ADB device.
 * Assumes ADB_PORT's DDR is set to input.
 */
uint16_t adb_read16() {
  // Timer/Counter0
  TCCR0A = 0; // defaults
  TCCR0B = 0; // defaults
  TCCR0B = 0b010 << CS00; // 8x prescaler

  uint8_t low_times[18];

  for (int i = 0; i < 18; i++) {
    while (ADB_PIN & ADB_DATA_MASK); // block while HIGH
    TCNT0 = 0;
    while ((ADB_PIN & ADB_DATA_MASK) == 0); // block while LOW
    low_times[i] = TCNT0;
  }

  uint16_t response = 0;

  for (int i = 1; i < 17; i++) { // ignore start/stop bit
    uint8_t lt = low_times[i] / (F_CPU / 8000000); // 8x prescaler
    response <<= 1;
    if (lt > 25 && lt < 45) response |= 1;
  }

  return response;
}

void adb_write16(uint16_t data) {
  adb_data_mode_output();
  _delay_us(150); // Tlt (stop-to-start time)
  adb_send_high(); // start-bit
  adb_send_byte((uint8_t)(data >> 8));
  adb_send_byte((uint8_t)data);
  adb_send_low(); // stop-bit
  adb_data_mode_input();
}

void adb_keyboard_animate_leds() {
  struct adb_cmd listen = {
    .address = ADB_KB_ADDRESS,
    .command = ADB_COMMAND_LISTEN,
    .reg = 2
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
    adb_send_command(listen);
    adb_write16(~states[i]);
    _delay_ms(50);
  }
}
