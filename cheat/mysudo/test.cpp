#include <stdlib.h>
#include <string>
#include <iostream>

using namespace std;

int main(int n, const char** p)
{
	if (n < 2) {
		cout << "args?" << endl;
		return 0;
	}
	string str = p[1];
	for (int i = 2; i < n; i++) {
		string arg = p[i];
		if (i > 1)
			str += " ";
		str += '\"' + arg + '\"';
	}
	system(str.c_str());
}