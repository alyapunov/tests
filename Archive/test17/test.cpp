#include <iostream>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include <inttypes.h>
#include <typeinfo>


int64_t ttime()
{
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts.tv_nsec + ts.tv_sec * 1000000000ll;
}

#ifdef __NO_INLINE__
#error please compile with -O2
#endif

#ifndef __OPTIMIZE__
#error please compile with -O2
#endif

static int deref[256] = {0};

using namespace std;

int f1(const char* s, int sz)
{
	int res = 0;
	for(int i = 0; i < sz; i++) {
		if(s[i] >= '0' && s[i] <= '9') {
			res = (res << 4) + (s[i] - '0');
		} else if(s[i] >= 'a' && s[i] <= 'f') {
			res = (res << 4) + 10 + (s[i] - 'a');
		} else if(s[i] >= 'A' && s[i] <= 'F') {
			res = (res << 4) + 10 + (s[i] - 'A');
		} else {
			res = (res << 4);
		}
	}
	return res;
}

int f2(const char* s, int sz)
{
	int res = 0;
	for(int i = 0; i < sz; i++) {
		switch(s[i]) {
			case '1': res = (res << 4) + 1; break;
			case '2': res = (res << 4) + 2; break;
			case '3': res = (res << 4) + 3; break;
			case '4': res = (res << 4) + 4; break;
			case '5': res = (res << 4) + 5; break;
			case '6': res = (res << 4) + 6; break;
			case '7': res = (res << 4) + 7; break;
			case '8': res = (res << 4) + 8; break;
			case '9': res = (res << 4) + 9; break;
			case 'a': res = (res << 4) + 10; break;
			case 'b': res = (res << 4) + 11; break;
			case 'c': res = (res << 4) + 12; break;
			case 'd': res = (res << 4) + 13; break;
			case 'e': res = (res << 4) + 14; break;
			case 'f': res = (res << 4) + 15; break;
			case 'A': res = (res << 4) + 10; break;
			case 'B': res = (res << 4) + 11; break;
			case 'C': res = (res << 4) + 12; break;
			case 'D': res = (res << 4) + 13; break;
			case 'E': res = (res << 4) + 14; break;
			case 'F': res = (res << 4) + 15; break;
			default : res <<= 4;
		}
	}
	return res;
}

int f3(const char* s, int sz)
{
	int res = 0;
	for(int i = 0; i < sz; i++) {
		res = (res << 4) + deref[(unsigned char)s[i]];
	}
	return res;
}


int main()
{
	for(int i = 0; i < 10; i++)
		deref[i + (unsigned char)'0'] = i;
	for(int i = 0; i < 6; i++)
		deref[i + (unsigned char)'a'] = 10 + i;
	for(int i = 0; i < 6; i++)
		deref[i + (unsigned char)'A'] = 10 + i;

	srand(time(0));
	char test_sym[] = "0123456789abcdefABCDEF";
	char test[16*1024];
	for(int i = 0; i < (int)sizeof(test); i++)
		test[i] = test_sym[rand() % sizeof(test_sym)];

	int64_t start, end;
	int step_size = 4;
	int check_sum = 0;

	check_sum = 0;
	start = ttime();
	for(int k = 0; k < 100; k++) {
		for(int i = 0; i < (int)sizeof(test); i += step_size)
			check_sum += f1(test + i, step_size);
	}
	end = ttime();
	cout << "test1: ifs: time=" << (end - start) / 1000 << " check=" << check_sum << endl;

	check_sum = 0;
	start = ttime();
	for(int k = 0; k < 100; k++) {
		for(int i = 0; i < (int)sizeof(test); i += step_size)
			check_sum += f2(test + i, step_size);
	}
	end = ttime();
	cout << "test2: switch: time= " << (end - start) / 1000 << " check=" << check_sum << endl;

	check_sum = 0;
	start = ttime();
	for(int k = 0; k < 100; k++) {
		for(int i = 0; i < (int)sizeof(test); i += step_size)
			check_sum += f3(test + i, step_size);
	}
	end = ttime();
	cout << "test3: arr: time=" << (end - start) / 1000  << " check=" << check_sum << endl;
}
