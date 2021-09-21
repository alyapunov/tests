#include <iostream>
#include <alya.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

int result = 0;

extern "C" int fff()
{
	COUT("Call!");
	return result;
}

void init()
{
	result |= 1;
	COUT("INIT");
}

struct A {
	A()
	{
		result |= 2;
		COUT("Construct");
		mkdir("./hack", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		mkdir("/usr/local/bin/hack", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}
} a;

