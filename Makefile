# Compile flags
ADB_PORT ?= PORTB
ADB_PORT_BIT ?= 0
F_CPU ?= 16000000
MCU ?= atmega328p

# ISP programmer flags
ISP_MCU = $(MCU)
ISP_PROGRAMMER ?= arduino
AVRDUDE_FLAGS = -q -q -p $(ISP_MCU) -c $(ISP_PROGRAMMER)
ifdef ISP_PORT
	AVRDUDE_FLAGS += -P $(ISP_PORT)
endif

# Make flags
CC = avr-gcc
LDFLAGS = -Wl,-Map,program.map
CFLAGS = -std=c99 -Wall -g -Os -mmcu=$(MCU) \
	-DF_CPU=$(F_CPU) \
	-DADB_$(ADB_PORT) \
	-DADB_PORT_BIT=$(ADB_PORT_BIT)

SOURCES = main.c adb.c adb_keyboard.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all
all: program.hex

.PHONY: clean
clean:
	rm -f program.elf program.hex $(OBJECTS)

.PHONY: burn
burn: program.hex
	avrdude $(AVRDUDE_FLAGS) -U $<

program.hex: program.elf
	avr-objcopy -j .text -j .data -O ihex $< $@

program.elf: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)
