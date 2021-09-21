#include <iostream>
#include <alya.h>
#include <string.h>
#include <math.h>

using namespace std;
using namespace alya;

const size_t size = 128 * 1024 * 1024;
unsigned char buf[size];

void
simple_process(uint32_t &res, unsigned char *&c)
{
	switch (*c) {
	case 0 ... 31:
		res += *c++;
		break;
	case 32 ... 35:
		if (*c == 32)
			res += (32 - *c) * *c;
		else if (*c == 33)
			res += (32 - *c) + *c;
		else if (*c == 34)
			res += (32 - *c) - *c;
		else
			res += (32 - *c) * *c * *c;
		c++;
		break;
	case 36 ... 39:
		if (*c == 36)
			res += (36 - *c) * *c;
		else if (*c == 37)
			res += (36 - *c) - *c;
		else if (*c == 38)
			res += (36 - *c) + *c;
		else
			res += (36 - *c) * *c * *c;
		c++;
		break;
	case 40 ... 43:
		if (*c == 40)
			res += (40 - *c) + *c;
		else if (*c == 41)
			res += (40 - *c) - *c;
		else if (*c == 42)
			res += (40 - *c) * *c;
		else
			res += (40 - *c) * *c * *c;
		c++;
		break;
	case 44 ... 47:
		if (*c == 44)
			res += (44 - *c) - *c;
		else if (*c == 45)
			res += (44 - *c) + *c;
		else if (*c == 46)
			res += (44 - *c) * *c;
		else
			res += (44 - *c) * *c * *c;
		c++;
		break;
	case 48 ... 51:
		if (*c == 48)
			res += (48 - *c) * *c;
		else if (*c == 49)
			res += (48 - *c) + *c;
		else if (*c == 50)
			res += (48 - *c) - *c;
		else
			res += (48 - *c) * *c * *c;
		c++;
		break;
	case 52 ... 55:
		if (*c == 52)
			res += (52 - *c) * *c;
		else if (*c == 53)
			res += (52 - *c) - *c;
		else if (*c == 54)
			res += (52 - *c) + *c;
		else
			res += (52 - *c) * *c * *c;
		c++;
		break;
	case 56 ... 59:
		if (*c == 56)
			res += (56 - *c) + *c;
		else if (*c == 57)
			res += (56 - *c) - *c;
		else if (*c == 58)
			res += (56 - *c) * *c;
		else
			res += (56 - *c) * *c * *c;
		c++;
		break;
	case 60 ... 63:
		if (*c == 60)
			res += (60 - *c) + *c;
		else if (*c == 61)
			res += (60 - *c) - *c;
		else if (*c == 62)
			res += (60 - *c) * *c;
		else
			res += (60 - *c) * *c * *c;
		c++;
		break;
	default:
		assert(false);
		break;
	}
}

static void process0(uint32_t &res, unsigned char *&c)
{
	res += *c++;
}

static void process32(uint32_t &res, unsigned char *&c)
{
	res += (32 - *c) * *c;
	c++;
}

static void process33(uint32_t &res, unsigned char *&c)
{
	res += (32 - *c) + *c;
	c++;
}

static void process34(uint32_t &res, unsigned char *&c)
{
	res += (32 - *c) - *c;
	c++;
}

static void process35(uint32_t &res, unsigned char *&c)
{
	res += (32 - *c) * *c * *c;
	c++;
}

static void process36(uint32_t &res, unsigned char *&c)
{
	res += (36 - *c) * *c;
	c++;
}

static void process37(uint32_t &res, unsigned char *&c)
{
	res += (36 - *c) - *c;
	c++;
}

static void process38(uint32_t &res, unsigned char *&c)
{
	res += (36 - *c) + *c;
	c++;
}

static void process39(uint32_t &res, unsigned char *&c)
{
	res += (36 - *c) * *c * *c;
	c++;
}

static void process40(uint32_t &res, unsigned char *&c)
{
	res += (40 - *c) + *c;
	c++;
}

static void process41(uint32_t &res, unsigned char *&c)
{
	res += (40 - *c) - *c;
	c++;
}

static void process42(uint32_t &res, unsigned char *&c)
{
	res += (40 - *c) * *c;
	c++;
}

static void process43(uint32_t &res, unsigned char *&c)
{
	res += (40 - *c) * *c * *c;
	c++;
}

static void process44(uint32_t &res, unsigned char *&c)
{
	res += (44 - *c) - *c;
	c++;
}

static void process45(uint32_t &res, unsigned char *&c)
{
	res += (44 - *c) + *c;
	c++;
}

static void process46(uint32_t &res, unsigned char *&c)
{
	res += (44 - *c) * *c;
	c++;
}

static void process47(uint32_t &res, unsigned char *&c)
{
	res += (44 - *c) * *c * *c;
	c++;
}

static void process48(uint32_t &res, unsigned char *&c)
{
	res += (48 - *c) * *c;
	c++;
}

static void process49(uint32_t &res, unsigned char *&c)
{
	res += (48 - *c) + *c;
	c++;
}

static void process50(uint32_t &res, unsigned char *&c)
{
	res += (48 - *c) - *c;
	c++;
}

static void process51(uint32_t &res, unsigned char *&c)
{
	res += (48 - *c) * *c * *c;
	c++;
}

static void process52(uint32_t &res, unsigned char *&c)
{
	res += (52 - *c) * *c;
	c++;
}

static void process53(uint32_t &res, unsigned char *&c)
{
	res += (52 - *c) - *c;
	c++;
}

static void process54(uint32_t &res, unsigned char *&c)
{
	res += (52 - *c) + *c;
	c++;
}

static void process55(uint32_t &res, unsigned char *&c)
{
	res += (52 - *c) * *c * *c;
	c++;
}

static void process56(uint32_t &res, unsigned char *&c)
{
	res += (56 - *c) + *c;
	c++;
}

static void process57(uint32_t &res, unsigned char *&c)
{
	res += (56 - *c) - *c;
	c++;
}

static void process58(uint32_t &res, unsigned char *&c)
{
	res += (56 - *c) * *c;
	c++;
}

static void process59(uint32_t &res, unsigned char *&c)
{
	res += (56 - *c) * *c * *c;
	c++;
}

static void process60(uint32_t &res, unsigned char *&c)
{
	res += (60 - *c) + *c;
	c++;
}

static void process61(uint32_t &res, unsigned char *&c)
{
	res += (60 - *c) - *c;
	c++;
}

static void process62(uint32_t &res, unsigned char *&c)
{
	res += (60 - *c) * *c;
	c++;
}

static void process63(uint32_t &res, unsigned char *&c)
{
	res += (60 - *c) * *c * *c;
	c++;
}

typedef void (*process_t)(uint32_t &res, unsigned char *&c);

uint32_t __attribute__((noinline)) test1(unsigned char* b)
{
	static process_t processes[64] = {
		process0, process0, process0, process0, process0, process0, process0, process0,
		process0, process0, process0, process0, process0, process0, process0, process0,
		process0, process0, process0, process0, process0, process0, process0, process0,
		process0, process0, process0, process0, process0, process0, process0, process0,
		process32, process33, process34, process35, process36, process37, process38, process39,
		process40, process41, process42, process43, process44, process45, process46, process47,
		process48, process49, process50, process51, process52, process53, process54, process55,
		process56, process57, process58, process59, process60, process61, process62, process63 };

	uint32_t res = 0;
	while (b != buf + size)
	{
		if (*b < 32)
			process0(res, b);
		else
			processes[*b](res, b);
	}
	return res;
}

uint32_t __attribute__((noinline)) test2(unsigned char* b)
{
	uint32_t res = 0;
	while (b != buf + size)
		simple_process(res, b);
	return res;
}

int
main(int, const char**)
{
	srand(0);
	for (size_t i = 0; i < size; i++) {
		buf[i] = rand() % 64;
	}

	CTimer t;
	uint32_t res;

	t.Start();
	res = test1(buf);
	t.Stop();
	COUTF("funcs", t.Mrps(size), res);

	t.Start();
	res = test2(buf);
	t.Stop();
	COUTF("switch", t.Mrps(size), res);
}
