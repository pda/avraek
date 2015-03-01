#include <avr/io.h>
#include <util/delay.h>

#define ADB_DATA_MASK (0b00000001)
#define ADB_HIGH() (PORTB |= ADB_DATA_MASK)
#define ADB_LOW() (PORTB &= ~ADB_DATA_MASK)

void setup();
void loop();

void adb_reset();
void adb_data_mode_input();
void adb_data_mode_output();

int main() {
  setup();
  while (1) loop();
}

void setup() {
  adb_reset();
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
