DEVICE=atmega328p
F_CPU = 16000000

CC=avr-gcc
CFLAGS=-mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -Os
PFLAGS=-p $(DEVICE) -c arduino -P /dev/ttyUSB0 -b 57600
PROJNAME=uhr2
#IFILE=$(PROJNAME).c
CSRC = lib/uart/uart.c
CPPSRC = main.cpp time.cpp lib/dcf77/DCF77.cpp
OFILE=$(PROJNAME).elf

# Define all object files.
OBJ = $(CSRC:.c=.o) $(CPPSRC:.cpp=.o)

all: $(OBJ)
	@echo "Compiling the program..."
	$(CC) $(CFLAGS) -o $(OFILE) $(OBJ)

# Compile: create object files from C source files.
%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o:%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(OFILE) $(OBJ)

flash:
	avrdude $(PFLAGS) -U flash:w:$(OFILE)
