libhot_fix.a: hot_fix.o
	ar -rv libhot_fix.a hot_fix.o

hot_fix.o: hot_fix.cpp hot_fix.h
	g++ -c -Wall hot_fix.cpp

clean: 
	rm -f libhot_fix.a hot_fix.o
