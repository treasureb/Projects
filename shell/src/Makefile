ROOT_PATH=$(shell pwd)
INCLUDE=$(ROOT_PATH)/../include
BIN=myshell
SRC=main.c init.c parse.c
OBJS =$(shell echo $(SRC) | sed 's/\.c/\.o/g')
CC=gcc
CFLAGS=-Wall -g

$(BIN):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o : %.c
	$(CC) -c $< -o $@ -I$(INCLUDE)

.PHONY:clean myshell
clean:
	rm -rf $(OBJS) $(BIN)

.PHONY:debug
debug:
	echo $(ROOT_PATH)
	echo $(INCLUDE)
