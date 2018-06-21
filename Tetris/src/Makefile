ROOT_PATH=$(shell pwd)
INCLUDE=$(ROOT_PATH)/../include
LIB=$(ROOT_PATH)/../lib
CC=gcc
FLAGS=-Wall -g
BIN=tetris
SRC=main.c tetris.c
OBJS=$(shell echo $(SRC) | sed 's/\.c/\.o/g')

$(BIN):$(OBJS)
	$(CC) $(FLAGS) -o $@ $(OBJS) $(LIB)/libkeyboard.a

%.o : %.c
	$(CC) -c $< -o $@ -I$(INCLUDE)

.PHOYN:clean
clean:
	rm $(BIN) $(OBJS)
