all : test_prog libtest_fix.so 
	@echo 'done'
	
test_prog : test_prog.cpp
	g++ -o $@ $^ -I../src -L../src -lhot_fix -rdynamic -ldl

libtest_fix.so : test_fix.cpp	
	g++ -shared -o$@ $^ -fPIC -I../src
	
clean :
	rm -f test_prog libtest_fix.so *.o
