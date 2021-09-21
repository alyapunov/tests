#include <iostream>
#include <alya.h>
#include <dlfcn.h>

using namespace std;


typedef int (*fff_t)();

int main(int, const char**)
{
	void *h = dlopen("./test.so",  RTLD_LAZY);
	if (!h) {
		COUT("Failed");
		return 1;
	}
	fff_t fff;
	fff = (fff_t)dlsym(h, "fff");
	if (!fff) {
		COUT("Failed (2)");
	} else {
		COUTF(fff());
	}
	dlclose(h);
}