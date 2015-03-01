#pragma once

#include <stdint.h>

// Makefile provides these as compiler flags:
// #define ADB_PORT PORTB
// #define ADB_DDR DDRB
// #define ADB_DATA_PIN

#define ADB_DATA_MASK (1 << ADB_DATA_PIN)

#define ADB_HIGH() (ADB_PORT |= ADB_DATA_MASK)
#define ADB_LOW() (ADB_PORT &= ~ADB_DATA_MASK)

#define ADB_COMMAND_TALK 0b11
#define ADB_KB_ADDRESS 0b0010
#define ADB_REGISTER_DEFAULT 0b00
#define ADB_REGISTER_INFO 0b11

struct adb_cmd {
  uint8_t address;
  uint8_t command;
  uint8_t reg;
};

void adb_reset();
void adb_data_mode_input();
void adb_data_mode_output();
void adb_send_command(struct adb_cmd);
void adb_command_packet(struct adb_cmd);
void adb_command_byte(uint8_t byte);
void adb_command_bit_low();
void adb_command_bit_high();
uint8_t adb_cmd_to_byte(struct adb_cmd);
