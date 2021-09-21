#include <iostream>
#include <alya.h>

using namespace std;
using namespace alya;

unsigned
bsc7(unsigned r)
{
	assert((r & ~0x7F) == 0);
	return ((r * 0x02040810 & 0x11111110) * 0x01111111) >> 28;
}

unsigned
bsc15(unsigned r)
{
	assert((r & ~0x7FFF) == 0);
	return ((r * 0x2000400080010ULL & 0x1111111111111110ULL) * 0x111111111111111ULL) >> 60;
}

unsigned
bsc(unsigned r)
{
	return bsc15(r & 0x7FFF) + bsc15((r >> 15) & 0x7FFF) + (r >> 30) - (r >> 31);
}

unsigned
ctz1(unsigned r)
{
	return __builtin_ctz(r);
}

unsigned
ctz2(unsigned r)
{
	for (unsigned res = 0; res < 32; res++)
		if (r & (1 << res))
			return res;
	return 32;
}

unsigned
ctz3(unsigned r)
{
	unsigned mask = 0xFFFFFFFF;
	unsigned res = 0;
	for (unsigned i = 16; i > 0; i >>= 1) {
		mask >>= i;
		if (!(r & mask)) {
			r >>= i;
			res += i;
		}
	}
	return res;
}

unsigned
ctz4(unsigned r)
{
	unsigned mask = 0xFFFFFFFF;
	unsigned res = 0;
	for (unsigned i = 16; i > 2; i >>= 1) {
		mask >>= i;
		if (!(r & mask)) {
			r >>= i;
			res += i;
		}
	}
	return res + ((0x12131210 >> (r << 1)) & 3);
}

unsigned
ctz5(unsigned r)
{
	unsigned u = r & 0xFFFF;
	unsigned res = u ? 0 : 16;
	u = u ? u : r >> 16;
	u |= (u >> 2) & 0x3333;
	u |= (u >> 1) & 0x1111;
	u &= 0x1111;
	u = (u * 0x12480000) >> 28;
	res += (((0x12131210 >> (u << 1)) & 3)) << 2;
	r = (r >> res) & 0xF;
	return res + ((0x12131210 >> (r << 1)) & 3);
}

const size_t test_size = 16 * 1024;
const size_t count = test_size * (test_size + 1) / 2;

unsigned arr[test_size];

int main(int, const char**)
{
	unsigned tmp = 0;
	for(unsigned r = 1; r < 16; r++) {
		tmp |= ctz1(r) << (r << 1);
	}
	printf("%X\n", tmp);

	for (unsigned r = 0; r < 0x80; r++) {
		unsigned r1 = __builtin_popcount(r);
		unsigned r2 = bsc7(r);
		if (r1 != r2) {
			COUTF("Error1", r, r1, r2);
		}
	}
	for (unsigned r = 0; r < 0x8000; r++) {
		unsigned r1 = __builtin_popcount(r);
		unsigned r2 = bsc15(r);
		if (r1 != r2) {
			COUTF("Error2", r, r1, r2);
		}
	}
	for (unsigned i = 0; i < 1000000; i++) {
		unsigned r = rand();
		unsigned r1 = __builtin_popcount(r);
		unsigned r2 = bsc(r);
		if (r1 != r2) {
			COUTF("Error3", r, r1, r2);
		}
	}

	for (unsigned i = 1; i < 1000000; i++) {
		unsigned ctzs[] = {ctz1(i), ctz2(i), ctz3(i), ctz4(i), ctz5(i)};
		bool match = true;
		for (size_t j = 1; j < sizeof(ctzs) / sizeof(ctzs[0]); j++)
			if (ctzs[j] != ctzs[0])
				match = false;
		if (!match) {
			std::cout << "failed at " << i << " : ";
			for (size_t j = 0; j < sizeof(ctzs) / sizeof(ctzs[0]); j++)
				std::cout << ctzs[j] << " ";
			std::cout << std::endl;
		}
	}

	CTimer t;
	unsigned res, Mrps;

	srand(time(0));
	for (size_t i = 0; i < test_size; i++) {
		while((arr[i] = rand()) == 0);
	}
	
	res = 0;
	t.Start();
	for (size_t i = 0; i < test_size; i++) {
		for (size_t j = i; j < test_size; j++) {
			unsigned r = arr[j];
			res ^= ctz1(r);
		}
	}
	Mrps = count / t.ElapsedMillisec() / 1000;
	COUTF(t.ElapsedMillisec(), count, Mrps, res);

	res = 0;
	t.Start();
	for (size_t i = 0; i < test_size; i++) {
		for (size_t j = i; j < test_size; j++) {
			unsigned r = arr[j];
			res ^= ctz2(r);
		}
	}
	Mrps = count / t.ElapsedMillisec() / 1000;
	COUTF(t.ElapsedMillisec(), count, Mrps, res);

	res = 0;
	t.Start();
	for (size_t i = 0; i < test_size; i++) {
		for (size_t j = i; j < test_size; j++) {
			unsigned r = arr[j];
			res ^= ctz3(r);
		}
	}
	Mrps = count / t.ElapsedMillisec() / 1000;
	COUTF(t.ElapsedMillisec(), count, Mrps, res);

	res = 0;
	t.Start();
	for (size_t i = 0; i < test_size; i++) {
		for (size_t j = i; j < test_size; j++) {
			unsigned r = arr[j];
			res ^= ctz4(r);
		}
	}
	Mrps = count / t.ElapsedMillisec() / 1000;
	COUTF(t.ElapsedMillisec(), count, Mrps, res);

	res = 0;
	t.Start();
	for (size_t i = 0; i < test_size; i++) {
		for (size_t j = i; j < test_size; j++) {
			unsigned r = arr[j];
			res ^= ctz5(r);
		}
	}
	Mrps = count / t.ElapsedMillisec() / 1000;
	COUTF(t.ElapsedMillisec(), count, Mrps, res);
}
