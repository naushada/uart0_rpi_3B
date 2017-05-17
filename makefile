BIN = UART0 
CC = arm-linux-gcc

OBJ_DIR = ../obj
CFLAGS = -g -c

INC = -I./gt-511c3/inc -I./pi3-uart/inc

SRC = $(shell find . -type f -name '*.c')

OBJ = $(SRC:.c=.o)


all: $(SRC) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@

.c.o:
	$(CC) $(CFLAGS) $(INC) $< -o $@


clean:
	rm -fr *.o
	rm -fr $(BIN)
