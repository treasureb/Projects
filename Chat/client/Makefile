.PHONY:all
all:test_win test_client

test_win:window.cc
	g++ -o $@ $^ -lncursesw
test_client:chat_client.cc
	g++ -o $@ $^ -lpthread -ljsoncpp -std=c++11
.PHOYN:clean
clean:
	rm -f test_client test_win
