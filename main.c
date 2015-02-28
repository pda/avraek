#include <avr/io.h>
#include <util/delay.h>

void setup();
void loop();

int main() {
  setup();
  while (1) loop();
}

void setup() {
  DDRB = 0xFF;
}

void loop() {
  PORTB = 0xFF;
  _delay_ms(500);
  PORTB = 0x00;
  _delay_ms(500);
}
