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

#ifndef ADB_PORT_PIN
  #error "ADB_PORT_PIN required"
#endif

#define ADB_DATA_MASK (1 << ADB_PORT_PIN)

#define ADB_HIGH() (ADB_PORT |= ADB_DATA_MASK)
#define ADB_LOW() (ADB_PORT &= ~ADB_DATA_MASK)

#define ADB_COMMAND_LISTEN 0b10
#define ADB_COMMAND_TALK 0b11
#define ADB_KB_ADDRESS 0b0010
#define ADB_REGISTER_DEFAULT 0b00
#define ADB_REGISTER_INFO 0b11

struct adb_cmd {
  uint8_t address;
  uint8_t command;
  uint8_t reg;
};

// Public
void adb_reset();
void adb_send_command(struct adb_cmd);
void adb_keyboard_initialize();

// Private
uint8_t adb_cmd_to_byte(struct adb_cmd);
void adb_command_data_16(uint16_t);
void adb_command_packet(struct adb_cmd);
void adb_data_mode_input();
void adb_data_mode_output();
void adb_keyboard_animate_leds();
uint16_t adb_receive_16();
void adb_send_byte(uint8_t byte);
void adb_send_high();
void adb_send_low();
