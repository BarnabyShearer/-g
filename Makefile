#
# Powder feed and leveler
#
# Copyright 2013 <b@Zi.iS>
# License GPLv2
#

.PHONY: clean upload flash run

MCU = atmega328p
F_CPU = 16000000
PORT = /dev/ttyUSB0 #/tmp/simavr-uart0
CC = avr-gcc

CFLAGS = -std=gnu99 -g3 -gdwarf-2 -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU)UL -Wl,-u,vfprintf -lprintf_flt -lm
OBJCOPY = avr-objcopy

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: main.hex

upload: main.hex
	#stty hup < $(PORT); true
	avrdude -p $(MCU) -P $(PORT) -c arduino -b57600 -U flash:w:$^:i

flash: main.hex
	avrdude -p $(MCU) -P usb -c avrisp2 -U flash:w:$^:i

run: main.hex
	simavr/simavr/run_avr -m $(MCU) -f $(F_CPU) $^

main.elf: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

clean:
	-rm *.o *.elf *.hex
