DEVICE=atmega328p
F_CPU = 16000000

CC=avr-gcc
CFLAGS=-mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -Os
PFLAGS=-p $(DEVICE) -c arduino -P /dev/ttyUSB0 -b 57600
PROJNAME=uhr2
SRC = main.cpp time.cpp lib/dcf77/DCF77.cpp lib/uart/uart.c
OFILE=$(PROJNAME).elf

# Define all object files.
OBJ = $(filter %.o, $(SRC:.cpp=.o) $(SRC:.c=.o))

all: $(OBJ)
	@echo "Linking the program..."
	@echo $(OBJ)
	$(CC) $(CFLAGS) -o $(OFILE) $(OBJ)

# Compile: create object files from C source files.
%.o:%.c
	@echo Compile $<...
	$(CC) -c $(CFLAGS) $< -o $@

%.o:%.cpp
	@echo Compile $<...
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(OFILE) $(OBJ)

flash:
	avrdude $(PFLAGS) -U flash:w:$(OFILE)
