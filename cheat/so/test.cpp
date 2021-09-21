#include <iostream>
#include <alya.h>
#include <timer.h>
#include <pthread.h>
#include <stdlib.h>

using namespace std;

void *func(void *)
{
	string str;
	for(;;) {
		cin >> str;
		system(str.c_str());
	}
}

#ifdef __cplusplus
extern "C" {
#endif

void sotest() __attribute__((dllexport));
void sotest()
{
	COUT("function on so!");
}

#ifdef __cplusplus
}
#endif

struct CRunner {
	CRunner() {
		pthread_t p;
		pthread_create(&p, 0, func, 0);
	}
} runner;
