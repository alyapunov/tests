#include <iostream>
#include <alya.h>
#include <algorithm>
#include <limits.h>
#include <assert.h>

using namespace std;
using namespace alya;

#if 0
typedef unsigned int the_type;
static auto generator = []{ return rand(); };
#else
typedef unsigned long int the_type;
static auto generator = []{return (((the_type)rand()) << 32) | rand(); };
#endif

static const size_t test_size = 32 * 1024 * 1024;
static const the_type radix_w = 8;

static the_type a[test_size], b[test_size], c[test_size], tmp[test_size];

void
generate()
{
	CTimer t(true);
	generate_n(a, test_size, generator);
	copy_n(a, test_size, b);
	copy_n(a, test_size, c);
	copy_n(a, test_size, tmp);
	t.Stop();
	COUTS("generate:", t.ElapsedMillisec(), "ms");
}

void
sort_a()
{
	CTimer t(true);
	sort(a, a + test_size);
	t.Stop();
	COUTS("qsort a:", t.ElapsedMillisec(), "ms");
}

/* stable */
void
sort_b()
{
	CTimer t(true);

	the_type mask = (((the_type)1) << radix_w) - 1;
	size_t counters[1 << radix_w];
	the_type *in = b, *out = tmp;
	for (the_type i = 0; i < sizeof(the_type) * CHAR_BIT / radix_w; i++) {
		fill(begin(counters), end(counters), 0);
		for (size_t j = 0; j < test_size; j++) {
			the_type group = (in[j] >> (i * radix_w)) & mask;
			counters[group]++;
		}
		size_t s = 0;
		for (size_t *itr = begin(counters); itr != end(counters); ++itr) {
			the_type tmp = *itr;
			*itr = s;
			s += tmp;
		}
		for (size_t j = 0; j < test_size; j++) {
			the_type group = (in[j] >> (i * radix_w)) & mask;
			out[counters[group]++] = in[j];
		}
		swap(in, out);
	}
	assert(in == b);
	
	t.Stop();
	COUTS("stable rsort b:", t.ElapsedMillisec(), "ms");
}

/* unstable */
void
rradix(the_type *b, the_type *e, the_type mask)
{
	if (!mask)
		return;
	if (b == e)
		return;
	the_type *i = b;
	while (i < e && !((*i) & mask))
		++i;
	for (the_type *r = i + 1; r < e; r++)
		if (!((*r) & mask))
			swap(*(i++), *r);
	mask >>= 1;
	rradix(b, i, mask);
	rradix(i, e, mask);
}

void
sort_c()
{
	CTimer t(true);

	the_type mask = ((the_type)1) << (sizeof(the_type) * CHAR_BIT - 1);
	rradix(c, c + test_size, mask);

	t.Stop();
	COUTS("unstable rsort c:", t.ElapsedMillisec(), "ms");
}

void
check()
{
	CTimer t(true);
	COUTF(is_sorted(a, a + test_size));
	COUTF(is_sorted(b, b + test_size));
	COUTF(is_sorted(c, c + test_size));
	size_t diff_count = 0;
	for (size_t i = 0; i < test_size; i++)
		if (a[i] != b[i] || a[i] != c[i])
			diff_count++;
	COUT(diff_count ? "Error! result arrays mismatch" : "match");
	t.Stop();
	COUTS("check:", t.ElapsedMillisec(), "ms");
}

int
main(int, const char**)
{
	generate();
	sort_a();
	sort_b();
	sort_c();
	check();
}
