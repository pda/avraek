MCU = atmega328p
F_CPU = 16000000
PROGRAMMER = arduino
PORT = /dev/cu.usbmodem14231

GCC_FLAGS = -std=c99 -Wall -g -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU)

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

main.elf: main.c
	avr-gcc $(GCC_FLAGS) -Wl,-Map,main.map -o main.elf main.c
