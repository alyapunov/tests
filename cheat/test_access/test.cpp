#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

const char *name = "/usr/local/bin/tmp_test";

int main(int, const char**)
{
	{
		fstream f(name, ios_base::trunc | ios_base::out);
		if (!f.is_open()) {
			cout << "failed to open file " << name << " (1)" << endl;
			exit(EXIT_FAILURE);
		}
		f << "test";
	}
	{
		fstream f(name, ios_base::in);
		if (!f.is_open()) {
			cout << "failed to open file " << name << " (2)" << endl;
			exit(EXIT_FAILURE);
		}
		string str;
		f >> str;
		if (str != "test") {
			cout << "wrong content of " << name << endl;
			exit(EXIT_FAILURE);
		}
		f << "test";
	}
	{
		if (remove(name) != 0) {
			cout << "failed to delete file " << name << endl;
			exit(EXIT_FAILURE);
		}
	}
	cout << "seems that you have write access to " << name << endl;
	cout << "trying to start root bash" << endl;
	execl("/bin/sh", "");
	exit(EXIT_SUCCESS);
}