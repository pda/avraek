#include <avr/io.h>
#include <util/delay.h>

#define ADB_DATA_MASK 0b00000001
#define ADB_HIGH() (PORTB |= ADB_DATA_MASK)
#define ADB_LOW() (PORTB &= ~ADB_DATA_MASK)
#define ADB_COMMAND_TALK 0b11
#define ADB_KB_ADDRESS 0b0010
#define ADB_REGISTER_DEFAULT 0b00
#define ADB_REGISTER_INFO 0b11

void setup();
void loop();

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

int main() {
  setup();
  while (1) loop();
}

void setup() {
  adb_reset();
  _delay_ms(1); // arbitrary
  struct adb_cmd cmd = {
    .address = ADB_KB_ADDRESS,
    .command = ADB_COMMAND_TALK,
    .reg = ADB_REGISTER_INFO
  };
  adb_send_command(cmd);
}

void loop() {
}

void adb_reset() {
  adb_data_mode_output();
  ADB_LOW();
  _delay_ms(3);
  adb_data_mode_input();
}

void adb_data_mode_input() {
  DDRB &= ~ADB_DATA_MASK;
  PORTB &= ~ADB_DATA_MASK; // hi-Z input
}

void adb_data_mode_output() {
  ADB_HIGH(); // output high to match pull-up resistor
  DDRB |= ADB_DATA_MASK;
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
