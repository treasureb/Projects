ROOT_PATH=$(shell pwd)
CONF=$(ROOT_PATH)/conf/server.conf
PLUGIN=$(ROOT_PATH)/plugin
WWWROOT=$(ROOT_PATH)/wwwroot
SQL=$(ROOT_PATH)/sql_connect
cc=gcc
FLAGS=-D_STDOUT_ # -D_DEBUG_#
LDFLAGS=-lpthread #-static
bin=httpd
src=httpd.c main.c
obj=$(shell echo $(src) | sed 's/\.c/\.o/g')

.PHONY:all
all:$(bin) cgi
cgi:
	cd $(WWWROOT)/cgi-bin;make;cp math_cgi $(ROOT_PATH);cd -
	cd $(SQL);make;cp insert_cgi $(ROOT_PATH);cp select_cgi $(ROOT_PATH);cd -

$(bin):$(obj)
	$(cc) -o $@ $^ $(LDFLAGS)
%.o:%.c
#$(obj):$(src)
	$(cc) -c $< $(FLAGS)

.PHONY:clean
clean:
	rm -rf *.o $(bin) output;cd $(WWWROOT)/cgi-bin;make chclean;cd -
	cd $(SQL);make clean;cd -;rm -f insert_cgi select_cgi math_cgi
.PHONY:output
output:
	mkdir -p output/wwwroot/cgi-bin
	mkdir -p output/conf
	cp httpd output
	cp math_cgi output/wwwroot/cgi-bin
	cp insert_cgi output/wwwroot/cgi-bin
	cp select_cgi output/wwwroot/cgi-bin
	cp wwwroot/index.html output/wwwroot
	cp $(PLUGIN)/ctl_server.sh output
	cp conf/server.conf output/conf
