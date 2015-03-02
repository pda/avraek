MCU = atmega328p
F_CPU = 16000000L
UART_BAUD = 9600
PROGRAMMER = arduino
PORT = /dev/cu.usbmodem14231

ADB_PORT = PORTB
ADB_PORT_PIN = 0

ADB_DEFINE_FLAGS = -DADB_$(ADB_PORT) -DADB_PORT_PIN=$(ADB_PORT_PIN)
CC_DEFINE_FLAGS = -DF_CPU=$(F_CPU) -DUART_BAUD=$(UART_BAUD) $(ADB_DEFINE_FLAGS)
CC_FLAGS = -std=c99 -Wall -g -Os -mmcu=$(MCU) $(CC_DEFINE_FLAGS)

.PHONY: all
all: main.hex

.PHONY: clean
clean:
	rm -f main.hex main.map main.o main.elf

.PHONY: burn
burn: main.hex
	avrdude -q -q -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -U main.hex

main.hex: main.elf
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex

main.elf: main.c adb.c
	avr-gcc $(CC_FLAGS) -Wl,-Map,main.map -o main.elf main.c adb.c avr-uart/uart.c
