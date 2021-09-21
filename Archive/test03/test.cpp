#include <alya.h>
#include <string>
#include <ostream>

enum enum1 {
	test1 = 1,
	test2
};

enum {
	test3 = 1,
	test4
};

OUT_ENUM(enum1, test1, test2);

int sum(int a, int b, int c) { return a + b + c; }

int main(int, char **p)
{
	std::string s("Test");
	int val = 42;

	COUT("Running ", p[0], " program on ", sizeof(void*) * 8, "-bit system.");
	COUTC(s.c_str(), s.size(), (int)s[0], val, &val);
	ECHO(s.c_str(), s.size(), (int)s[0], val, &val);
	COUTS(0, test1, test2, ((enum1)3), 4);
	COUTS(0, test3, test4, 4); // wrong behaviour on undeclared enums
	COUTF(255, HEX(255), HEx(255), 255);
	COUTC(true, false, BOOL(true), BOOL(false));
	COUT('!', FMT(5, 10), '!', FMT(0.1, 10), '!', FMT(0.1, 10, 5), '!', FMT(10003.1415, 0, 2), '!');
	COUTF(BIN(1), BIN(0), BIN(-1), BIN(257), BIN(16), BIN(48));
	COUTF(BIN(-1ll));
	COUTF(BIN(0, 3), BIN(1, 3), BIN(256, 3), BIN(2, 5));
}
