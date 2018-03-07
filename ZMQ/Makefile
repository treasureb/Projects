.PHONY:all
all:client server

client:client.c
	gcc -o $@ $^ -lzmq
server:server.c
	gcc -o $@ $^ -lzmq

.PHONY:clean
clean:
	rm -f client server
