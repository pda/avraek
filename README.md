ADB Apple Desktop Bus keyboard adapter
======================================

Aims to use a cheap microcontroller to expose an Apple Extended Keyboard II
as a USB keyboard.

Hardware
--------

Single ADB data line connects to bottom-left pin of keyboard Mini-DIN with 1K pull-up resistor.

ATtiny328p (Arduino Uno)
------------------------

Defaults ADB to `PORTB` bit `0` which is ATmega328p pin 14, and Arduino digital
pin 8. See `Makefile` to override this using environment variables.

```sh
make clean all burn
```

ATtiny85
--------

Set to internal 16.5 MHz frequency like http://www.obdev.at/products/vusb/easylogger.html

```
# Fuse high byte:
# 0xdd = 1 1 0 1   1 1 0 1
#        ^ ^ ^ ^   ^ \-+-/ 
#        | | | |   |   +------ BODLEVEL 2..0 (brownout trigger level -> 2.7V)
#        | | | |   +---------- EESAVE (preserve EEPROM on Chip Erase -> not preserved)
#        | | | +-------------- WDTON (watchdog timer always on -> disable)
#        | | +---------------- SPIEN (enable serial programming -> enabled)
#        | +------------------ DWEN (debug wire enable)
#        +-------------------- RSTDISBL (disable external reset -> enabled)
#
# Fuse low byte:
# 0xe1 = 1 1 1 0   0 0 0 1
#        ^ ^ \+/   \--+--/
#        | |  |       +------- CKSEL 3..0 (clock selection -> HF PLL)
#        | |  +--------------- SUT 1..0 (BOD enabled, fast rising power)
#        | +------------------ CKOUT (clock output on CKOUT pin -> disabled)
#        +-------------------- CKDIV8 (divide clock by 8 -> don't divide)
```

```sh
avrdude -p attiny85 -c usbtiny -U hfuse:w:0xdd:m -U lfuse:w:0xe1:m
```

Build the code, use `PORTB` bit `3` which is ATtiny85 pin 2:

```sh
MCU=attiny85 F_CPU=16500000 ISP_PROGRAMMER=usbtiny ADB_PORT_BIT=3 make clean all burn
```
