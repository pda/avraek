#include "adb.h"
#include <avr/io.h>
#include <util/delay.h>

void adb_reset() {
  adb_data_mode_output();
  ADB_LOW();
  _delay_ms(3);
  adb_data_mode_input();
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
  adb_command_byte(adb_cmd_to_byte(cmd));
  adb_command_bit_low(); // stop-bit
}

void adb_command_byte(uint8_t byte) {
  for (int i = 0; i < 8; i++) {
    if (byte & (1 << 7)) {
      adb_command_bit_high();
    } else {
      adb_command_bit_low();
    }
    byte <<= 1;
  }
}

void adb_command_bit_low() {
  ADB_LOW();
  _delay_us(65);
  ADB_HIGH();
  _delay_us(35);
}

void adb_command_bit_high() {
  ADB_LOW();
  _delay_us(35);
  ADB_HIGH();
  _delay_us(65);
}

uint8_t adb_cmd_to_byte(struct adb_cmd cmd) {
  return (cmd.address << 4) | (cmd.command << 2) | (cmd.reg);
}
