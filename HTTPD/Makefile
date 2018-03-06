ROOT_PATH=$(shell pwd)
CONF=$(ROOT_PATH)/conf
PLUGIN=$(ROOT_PATH)/plugin
WWWROOT=$(ROOT_PATH)/wwwroot
cc=gcc
FLAGS=-D_STDOUT_ # -D_DEBUG_#
LDFLAGS=-lpthread #-static
bin=httpd
src=httpd.c main.c

obj=$(shell echo $(src) | sed 's/\.c/\.o/g')
$(bin):$(obj)
	$(cc) -o $@ $^ $(LDFLAGS)
%.o:%.c
#$(obj):$(src)
	$(cc) -c $< $(FLAGS)

.PHONY:clean
clean:
	rm *.o $(bin)
