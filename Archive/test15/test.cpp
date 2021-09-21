#include <algorithm>
#include <iostream>
#include <alya.h>
#include <msgpuck.h>

using namespace std;
using namespace alya;

#define FOR(type, what, from, upto) for (type what = from; what < upto; ++what)

const size_t ROUNDS = 16;
const size_t COUNT = 1024 * 1024;
uint64_t idata[COUNT];
uint64_t idata_res[COUNT];
char cdata[COUNT * 9];
char *cdata_end;

char *
my_encode_uint1(char *p, uint64_t val)
{
	unsigned char mask = (unsigned char)(uint64_t)(((int64_t)(val >> 7) - 1) >> 63);
	int b = 63 - __builtin_clzl((val & ~127ull) | 15);
	int c = 32 - __builtin_clz(b);
	int d = ((1 << c) >> 3);
	*p = (char)((mask & (unsigned char)val) | ((~mask) & (0xc9 + c)));
	p++;
	*(uint64_t *)p = __builtin_bswap64(val) >> (64 - d * 8);
	return p + d;
}

char *
my_encode_uint2(char *p, uint64_t val)
{
	if (val < 128) {
		*p = val;
		return p + 1;
	}
	int b = 63 - __builtin_clzl((val & ~127ull) | 15);
	int c = 32 - __builtin_clz(b);
	int d = ((1 << c) >> 3);
	*p = 0xc9 + c;
	p++;
	uint64_t magic_val = __builtin_bswap64(val) >> (64 - d * 8);
	memcpy(p, &magic_val, d);
	return p + d;
}

char *
my_encode_uint3(char *p, uint64_t val)
{
	int rating = 63 ^ __builtin_clzl(val | 1);
	switch(rating) {
	case 0 ... 6:
		*p = val;
		return p + 1;
	case 7:
		*p = 0xcc;
		*(uint8_t *)(p + 1) = val;
		return p + 2;
	case 8 ... 15:
		*p = 0xcd;
		*(uint16_t *)(p + 1) = mp_bswap_u16(val);
		return p + 3;
	case 16 ... 31:
		*p = 0xce;
		*(uint32_t *)(p + 1) = mp_bswap_u32(val);
		return p + 5;
	case 32 ... 63:
		*p = 0xcf;
		*(uint64_t *)(p + 1) = mp_bswap_u64(val);
		return p + 9;
	default:
		__builtin_unreachable();
	}
}

uint8_t bswap(uint8_t i) { return i; }
uint16_t bswap(uint16_t i) { return mp_bswap_u16(i); }
uint32_t bswap(uint32_t i) { return mp_bswap_u32(i); }
uint64_t bswap(uint64_t i) { return mp_bswap_u64(i); }

char *h0(char *p, uint64_t val)
{
	*p = val;
	return p + 1;
}

char utag(uint8_t) { return 0xcc; }
char utag(uint16_t) { return 0xcd; }
char utag(uint32_t) { return 0xce; }
char utag(uint64_t) { return 0xcf; }

template <class T>
char *h(char *p, uint64_t val)
{
	T tmp = val;
	tmp = bswap(tmp);
	*p = utag(tmp);
	memcpy(p + 1, &tmp, sizeof(T));
	return p + 1 + sizeof(T);
}

char *h1(char *p, uint64_t val) { return h<uint8_t>(p, val); }
char *h2(char *p, uint64_t val) { return h<uint16_t>(p, val); }
char *h3(char *p, uint64_t val) { return h<uint32_t>(p, val); }
char *h4(char *p, uint64_t val) { return h<uint64_t>(p, val); }

typedef char *(*helper_t)(char *p, uint64_t val);

char *
my_encode_uint4(char *p, uint64_t val)
{
	int rating = 63 ^ __builtin_clzl(val | 1);
	static const helper_t helpers[64] = {
		h0, h0, h0, h0, h0, h0, h0, h1,  h2, h2, h2, h2, h2, h2, h2, h2,
		h3, h3, h3, h3, h3, h3, h3, h3,  h3, h3, h3, h3, h3, h3, h3, h3,
		h4, h4, h4, h4, h4, h4, h4, h4,  h4, h4, h4, h4, h4, h4, h4, h4,
		h4, h4, h4, h4, h4, h4, h4, h4,  h4, h4, h4, h4, h4, h4, h4, h4
	};
	return helpers[rating](p, val);
}

struct info {
	uint64_t mask1;
	uint64_t tag1;
	uint64_t mask2;
	uint64_t size2;
	int shift2;
};

char *
my_encode_uint5(char *p, uint64_t val)
{
	int rating = 63 ^ __builtin_clzl(val | 1);

	static const struct info info0 = {0x7F, 0x00, 0xFFFFFFFFFFFFFFFFull, 0, 63};
	static const struct info info1 = {0x00, 0xCC, 0xFFFFFFFFFFFFFF00ull, 1, 56};
	static const struct info info2 = {0x00, 0xCD, 0xFFFFFFFFFFFF0000ull, 2, 48};
	static const struct info info3 = {0x00, 0xCE, 0xFFFFFFFF00000000ull, 4, 32};
	static const struct info info4 = {0x00, 0xCF, 0x0000000000000000ull, 8, 0};
	static const struct info infos[64] =
		{
			info0, info0, info0, info0, info0, info0, info0, info1,
			info2, info2, info2, info2, info2, info2, info2, info2,
			info3, info3, info3, info3, info3, info3, info3, info3,
			info3, info3, info3, info3, info3, info3, info3, info3,
			info4, info4, info4, info4, info4, info4, info4, info4,
			info4, info4, info4, info4, info4, info4, info4, info4,
			info4, info4, info4, info4, info4, info4, info4, info4,
			info4, info4, info4, info4, info4, info4, info4, info4
		};

	const struct info *that = infos + rating;
	*p = (val & that->mask1) | that->tag1;
	p++;
	uint64_t tmp;
	memcpy(&tmp, p, 8);
	tmp = (tmp & that->mask2) | (mp_bswap_u64(val) >> that->shift2);
	memcpy(p, &tmp, 8);
	p += that->size2;
	return p;
}

char *
my_encode_uint6(char *p, uint64_t val)
{
	unsigned char mask = (unsigned char)(uint64_t)(((int64_t)(val >> 7) - 1) >> 63);
	int b = 63 - __builtin_clzl((val & ~127ull) | 15);
	int c = 32 - __builtin_clz(b);
	int d = ((1 << c) >> 3);
	*p = (char)((mask & (unsigned char)val) | ((~mask) & (0xc9 + c)));
	p++;
	uint64_t tmp;
	memcpy(&tmp, p, 8);
	tmp &= 0xFFFFFFFFFFFFFFFFull << d * 8;
	tmp |= __builtin_bswap64(val) >> (64 - d * 8);
	memcpy(p, &tmp, 8);
	return p + d;
}

typedef char * (*enc_f)(char *p, uint64_t val);
typedef uint64_t (*dec_f)(const char **p);
enum workload_t {
	BRANCHLESS,
	BRANCHFULL,
};

OUT_ENUM(workload_t, BRANCHLESS, BRANCHFULL);

#define report_time(tm) do { double Mrps = ROUNDS * COUNT / double(tm); COUTF(__func__, tm, Mrps); } while(0)

void gen(workload_t wl)
{
	srand(0);
	static constexpr uint64_t LIMS[5] = {0, 128, 0x100ull, 0x10000ull, 0x100000000ull};
	for (size_t i = 0; i < COUNT; i++) {
		if (wl == BRANCHLESS) {
			idata[i] = rand();
			idata[i] *= 1024 * 1024;
			idata[i] ^= rand();
		} else {
			int t = rand() % 5;
			if (t == 0) {
				idata[i] = rand();
				idata[i] *= 1024 * 1024;
				idata[i] ^= rand();
			} else {
				idata[i] = LIMS[t-1] + rand() % LIMS[t];
			}
		}
	}
}

template <enc_f ENC>
__attribute__((noinline))
void do_enc(uint64_t *data, uint64_t *data_end)
{
	cdata_end = cdata;
	while (data != data_end) {
		cdata_end = ENC(cdata_end, *data);
		++data;
	}
}

template <dec_f DEC>
__attribute__((noinline))
uint64_t *do_dec(const char *data, const char *data_end)
{
	uint64_t *res = idata_res;
	while (data < data_end) {
		*res = DEC(&data);
		++res;
	}
	return res;
}

void fail(const char *where, const char* mess)
{
	std::cerr << mess << " in " << where << std::endl;
	abort();
}

void check(const char *where, uint64_t *dec_res)
{
	if (dec_res != idata_res + COUNT)
		fail(where, "wrong result number of ints");
	for (size_t i = 0; i < COUNT; i++)
		if (idata[i] != idata_res[i])
			fail(where, "wrong result int");
}

template <workload_t WORKLOAD, enc_f ENC, dec_f DEC>
void test(const char *name)
{
	gen(WORKLOAD);
	double max_enc_mrps = 0;
	double max_dec_mrps = 0;
	for (size_t k = 0; k < ROUNDS; k++) {
		CTimer t;

		t.Start();
		do_enc<ENC>(idata, idata + COUNT);
		t.Stop();
		max_enc_mrps = std::max(max_enc_mrps, t.Mrps(COUNT));

		t.Start();
		uint64_t *dec_res = do_dec<DEC>(cdata, cdata_end);
		t.Stop();
		max_dec_mrps = std::max(max_dec_mrps, t.Mrps(COUNT));

		check(name, dec_res);
	}
	size_t side1 = cdata_end - cdata;
	size_t side2 = 0;
	for (size_t i = 0; i < COUNT; i++)
		side2 ^= idata_res[i];
	COUTF(name, WORKLOAD, max_enc_mrps, max_dec_mrps, side1, side2);
}

/*
void
test_standart()
{
	srand(0);
	uint64_t nums[COUNT];
	FOR(size_t, j, 0, COUNT) {
		if (j % 3 == 0)
			nums[j] = rand();
		else if (j % 3 == 1)
			nums[j] = rand() % 32000;
		else
			nums[j] = rand() % 255;
	}

	CTimer t(true);
	CTimer t1, t2;
	char buf[COUNT * 10];
	size_t total_size = 0;
	long encode_time = 0;
	long decode_time = 0;
	FOR(size_t, i, 0, ROUNDS) {
		t1.Start();
		char *p1 = buf;
		uint64_t check_sum1 = 0;
		FOR(size_t, j, 0, COUNT) {
			uint64_t val = nums[j];
			check_sum1 ^= val;
			p1 = mp_encode_uint(p1, val);
		}
		t1.Stop();
		encode_time += t1.ElapsedMicrosec();

		total_size += p1 - buf;

		t2.Start();
		const char *p2 = buf;
		uint64_t check_sum2 = 0;
		FOR(size_t, j, 0, COUNT) {
			uint64_t val = mp_decode_uint(&p2);
			check_sum2 ^= val;
		}
		t2.Stop();
		decode_time += t2.ElapsedMicrosec();


		if (check_sum1 != check_sum2)
			COUTF(check_sum1, check_sum2);
	}
	t.Stop();
	COUTF(total_size);
	report_time(t.ElapsedMicrosec());
	report_time(encode_time);
	report_time(decode_time);
}
*/

template <workload_t WORKLOAD>
void test_all()
{
	test<WORKLOAD, mp_encode_uint, mp_decode_uint>("msgpuck");
	test<WORKLOAD, my_encode_uint1, mp_decode_uint>("my var1");
	test<WORKLOAD, my_encode_uint2, mp_decode_uint>("my var2");
	test<WORKLOAD, my_encode_uint3, mp_decode_uint>("my var3");
	test<WORKLOAD, my_encode_uint4, mp_decode_uint>("my var4");
	test<WORKLOAD, my_encode_uint5, mp_decode_uint>("my var5");
	test<WORKLOAD, my_encode_uint6, mp_decode_uint>("my var6");
}


template <size_t N>
struct Test {
    static size_t test(Test *) { return N; }
};

int main(int, const char**)
{
	COUTF(INT32_MIN);
	COUTF(INT32_MIN * -1);
	COUTF(INT32_MIN / -1);
	Test<3> x;
	COUTF(x.test(&x));
	return 0;

	uint64_t t = 0x1000010100;
	for (uint64_t i = 0; i < 10; i++) {
		COUTF(BIN(i), BIN(t ^ (t - i - 1)));
	}
	for (uint64_t i = 120; i < 130; i++) {
		COUTF(BIN(i), BIN(t ^ (t - i - 1)));
	}
	for (uint64_t i = 250; i < 460; i++) {
		COUTF(BIN(i), BIN(t ^ (t - i - 1)));
	}
	for (uint64_t i = 65530; i < 65540; i++) {
		COUTF(BIN(i), BIN(t ^ (t - i - 1)));
	}
	return 0;

	//test_standart();
	test_all<BRANCHLESS>();
	test_all<BRANCHFULL>();
}
