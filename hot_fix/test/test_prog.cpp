#include <iostream>

using namespace std;

#include <unistd.h>
#include <assert.h>
#include <signal.h>

#include "hot_fix.h"

int g_i = 100;

int add(int a, int b)
{
	return a - b + g_i;
}

int func(int a, int b)
{
	return add(a, b);
}

int main()
{
	assert(0 == init(SIGUSR1, "/home/ubuntu/hot_fix/test/libtest_fix.so"));
	while (true) {
		cout << add(1, 2) << ":" << func(3, 2) << endl;
		sleep(1);
	}
	return 0;
}

//kill -USR1 `ps -ef|grep test_prog|grep -v grep|awk '{print $2}'`
