#pragma once

#include "iocompat.h"

#include <stdint.h>

// Constants

// ADB protocol constants
#define ADB_COMMAND_LISTEN 0b10
#define ADB_COMMAND_TALK 0b11
#define ADB_KB_ADDRESS 0b0010
#define ADB_REGISTER_PRIMARY 0b00
#define ADB_REGISTER_1 0b01
#define ADB_REGISTER_2 0b10
#define ADB_REGISTER_IDENTITY 0b11

// Types

struct adb_cmd {
  uint8_t address;
  uint8_t command;
  uint8_t reg;
};

struct adb_response_16 {
  uint8_t timed_out;
  uint8_t a;
  uint8_t b;
};


// Functions

void adb_command_data_16(uint16_t);
void adb_receive_16(struct adb_response_16 *);
void adb_reset();
void adb_send_command(struct adb_cmd *);
