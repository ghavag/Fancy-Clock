DEVICE=atmega328p
F_CPU = 16000000

CC=avr-gcc
CFLAGS=-mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -Os
PFLAGS=-p $(DEVICE) -c arduino -P /dev/ttyUSB0 -b 57600
PROJNAME=uhr2
#IFILE=$(PROJNAME).c
SRC = main.cpp time.cpp lib/uart/uart.c lib/dcf77/DCF77.cpp
OFILE=$(PROJNAME).elf

all:
	$(CC) $(CFLAGS) -o $(OFILE) $(SRC)

clean:
	rm $(OFILE)

flash:
	avrdude $(PFLAGS) -U flash:w:$(OFILE)
