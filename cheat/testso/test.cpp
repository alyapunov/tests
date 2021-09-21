#include <iostream>
#include <alya.h>
#include <timer.h>
#include <unistd.h>
/*
#include <dlfcn.h>
*/

using namespace std;

void sotest() __attribute__((dllimport));

int main(int, const char**)
{
	sotest();

/*
	void *lib = dlopen("./libso.so", RTLD_NOW);
	COUTF(lib);
	void *s = dlsym(lib, "sotest");
	COUTF(s);
	void (*f)() = (void(*)()) s;
	f();
*/

	COUT("main goes to sleep");
	sleep(60);
	COUT("main sleep done");
}