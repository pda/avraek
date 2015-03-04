#pragma once

#include <stdint.h>

#ifdef ADB_PORTA
  #define ADB_PORT PORTA
  #define ADB_PIN PINA
  #define ADB_DDR DDRA
#elif ADB_PORTB
  #define ADB_PORT PORTB
  #define ADB_PIN PINB
  #define ADB_DDR DDRB
#elif ADB_PORTC
  #define ADB_PORT PORTC
  #define ADB_PIN PINC
  #define ADB_DDR DDRC
#elif ADB_PORTD
  #define ADB_PORT PORTD
  #define ADB_PIN PIND
  #define ADB_DDR DDRD
#elif ADB_PORTE
  #define ADB_PORT PORTE
  #define ADB_PIN PINE
  #define ADB_DDR DDRE
#elif ADB_PORTF
  #define ADB_PORT PORTF
  #define ADB_PIN PINF
  #define ADB_DDR DDRF
#else
  #error "ADB_PORT{A,B,C,D,E,F} required"
#endif

#ifndef ADB_PORT_BIT
  #error "ADB_PORT_BIT required"
#endif

#define ADB_DATA_MASK (1 << ADB_PORT_BIT)

#define ADB_HIGH() (ADB_PORT |= ADB_DATA_MASK)
#define ADB_LOW() (ADB_PORT &= ~ADB_DATA_MASK)
#define ADB_IS_HIGH (ADB_PIN & ADB_DATA_MASK)
#define ADB_IS_LOW (!(ADB_PIN & ADB_DATA_MASK))

#define ADB_COMMAND_LISTEN 0b10
#define ADB_COMMAND_TALK 0b11
#define ADB_KB_ADDRESS 0b0010
#define ADB_REGISTER_DEFAULT 0b00
#define ADB_REGISTER_INFO 0b11

// Types

struct adb_cmd {
  uint8_t address;
  uint8_t command;
  uint8_t reg;
};

// Functions

static uint8_t adb_cmd_to_byte(struct adb_cmd);
static void adb_command_data_16(uint16_t);
static void adb_command_packet(struct adb_cmd);
static void adb_data_mode_input();
static void adb_data_mode_output();
static void adb_keyboard_animate_leds();
static uint8_t adb_measure_time_until_high();
static uint16_t adb_receive_16();
static void adb_send_byte(uint8_t byte);
static void adb_send_command(struct adb_cmd);
static void adb_send_high();
static void adb_send_low();
static uint8_t adb_wait_for_low_with_timeout();
