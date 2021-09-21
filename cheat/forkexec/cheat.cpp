#include <iostream>
#include <alya.h>
#include <timer.h>
#include <unistd.h>

using namespace std;

int main(int, const char**)
{
	int f = fork();
	if (f == 0) {
		execl("/bin/ping", "ping", "localhost", NULL);
	}
	sleep(5);
}
