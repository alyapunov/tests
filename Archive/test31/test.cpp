#include <alya.h>
#include <array>
#include <vector>
#include <string>
#include <fstream>
#include <thread>
#include <cstring>

#define die() do { assert(false); abort(); } while(0)


template <bool IS_CONST, class T>
struct UniInt {
	static_assert(std::is_integral_v<T>);
	static_assert(!IS_CONST);
	static constexpr bool is_const = false;
	using type = T;
	T value;
	UniInt(T t) : value(t) {}
};

template <class T, T VALUE>
struct UniInt<true, std::integral_constant<T, VALUE>> {
	static_assert(std::is_integral_v<T>);
	static constexpr bool is_const = true;
	using type = T;
	static constexpr T value = VALUE;
};

template <class T>
UniInt<false, T>
uni_int(T t)
{
	return UniInt<false, T>{t};
}

template <class T, T VALUE>
UniInt<true, std::integral_constant<T, VALUE>>
uni_int(std::integral_constant<T, VALUE>)
{
	return UniInt<true, std::integral_constant<T, VALUE>>{};
}

using namespace std;

struct Node {
	size_t value;
	size_t hint;
	bool operator<(const Node& a) const { return hint < a.hint; }
};

struct Path {
	uint32_t value;
	uint32_t orig_pos;
	bool operator<(const Path& a) const { return value < a.value; }
};

using STAT_T = unsigned;

constexpr int RADIX_BITS = 16;
constexpr size_t RADIX_SIZE = 1ull << RADIX_BITS;
constexpr size_t RADIX_ROUNDS = (64 + RADIX_BITS - 1) / RADIX_BITS;
constexpr size_t NUM_THREADS = 1;

//constexpr size_t N = 32 * 1024 * 1024;
constexpr size_t N = 32 * 1024 * 1024;


Node test[N];
Node test0[N];
uint64_t mask[(N + 63)/ 64];
Path path[N];

void __attribute__((noinline)) work_thread_f0(size_t n, Node *src_arr, Node *dst_arr, size_t num, STAT_T *stat, size_t l)
{
	size_t i1 = n * num / NUM_THREADS;
	size_t i2 = (n + 1) * num / NUM_THREADS;
	constexpr size_t PRE_SIZE = 64;
	size_t pre_dst[PRE_SIZE];
//#define MINTRACK
#ifdef MINTRACK
	size_t total_prepare1 = 0;
				size_t total_prepare2 = 0;
				size_t total_move = 0;
#endif
	for (size_t i = i1; i < i2; i += PRE_SIZE) {
		if (i2 - i >= PRE_SIZE) {
#ifdef MINTRACK
			alya::CTimer t2;
			t2.Start();
#endif
			for (size_t j = 0; j < PRE_SIZE; j++) {
				size_t x = src_arr[i + j].hint;
				x >>= (l * RADIX_BITS);
				pre_dst[j] = x % RADIX_SIZE;
				__builtin_prefetch(stat + x, 1);
			}
#ifdef MINTRACK
			t2.Stop();
			total_prepare1 += t2.ElapsedMicrosec();
			t2.Start();
#endif
			for (size_t j = 0; j < PRE_SIZE; j++) {
				pre_dst[j] = stat[pre_dst[j]]++;
				__builtin_prefetch(dst_arr + pre_dst[j], 1);
			}
#ifdef MINTRACK
			t2.Stop();
			total_prepare2 += t2.ElapsedMicrosec();
			t2.Start();
#endif
			for (size_t j = 0; j < PRE_SIZE; j++)
				dst_arr[pre_dst[j]] = src_arr[i + j];
#ifdef MINTRACK
			t2.Stop();
			total_move += t2.ElapsedMicrosec();
#endif
		} else {
			size_t sz = i2 - i;
			for (size_t j = 0; j < sz; j++) {
				size_t x = src_arr[i + j].hint;
				x >>= (l * RADIX_BITS);
				pre_dst[j] = stat[x % RADIX_SIZE]++;
				__builtin_prefetch(dst_arr + pre_dst[j], 1);
			}
			for (size_t j = 0; j < sz; j++)
				dst_arr[pre_dst[j]] = src_arr[i + j];
		}
	}
#ifdef MINTRACK
	COUTF(total_prepare1, total_prepare2, total_move);
#endif
}

void __attribute__((noinline)) work_thread_f(size_t n, Node *src_arr, Node *dst_arr, size_t num, STAT_T *stat, size_t l)
{
	size_t i1 = n * num / NUM_THREADS;
	size_t i2 = (n + 1) * num / NUM_THREADS;
	for (size_t i = i1; i < i2; i++) {
		size_t x = src_arr[i].hint;
		x >>= (l * RADIX_BITS);
		dst_arr[stat[x % RADIX_SIZE]++] = src_arr[i];
	}
}

void
radix_mt(Node *arg_arr, size_t num)
{
	Node *src_arr = arg_arr;
	Node *dst_arr = NULL;
//#define TIMERS
#ifdef TIMERS
	alya::CTimer t;
#endif
	for (size_t l = 0; l < RADIX_ROUNDS; l++) {
#ifdef TIMERS
		t.Start();
#endif
		STAT_T stat[NUM_THREADS][RADIX_SIZE] = {};
		{
			auto thread_f = [&](size_t n) {
				size_t i1 = n * num / NUM_THREADS;
				size_t i2 = (n + 1) * num / NUM_THREADS;
				for (size_t i = i1; i < i2; i++) {
					size_t x = src_arr[i].hint;
					x >>= (l * RADIX_BITS);
					x %= RADIX_SIZE;
					stat[n][x]++;
				}
			};
			std::thread threads[NUM_THREADS];
			for (size_t i = 0; i < NUM_THREADS - 1; i++)
				threads[i] = std::thread(thread_f, i);
			thread_f(NUM_THREADS - 1);
			for (size_t i = 0; i < NUM_THREADS - 1; i++)
				threads[i].join();
		}
#ifdef TIMERS
		t.Stop();
		COUTF("Stat part", t.ElapsedMicrosec());

		t.Start();
#endif
		size_t num_nonzero = 0;
		for (size_t j = 0; j < RADIX_SIZE; j++) {
			size_t total = 0;
			for (size_t p = 0; p < NUM_THREADS; p++)
				total += stat[p][j];
			if (total != 0)
				num_nonzero++;
		}
		if (num_nonzero == 1) {
			continue;
		}

		size_t b = 0;
		for (size_t j = 0; j < RADIX_SIZE; j++) {
			for (size_t p = 0; p < NUM_THREADS; p++) {
				size_t tmp = b;
				b += stat[p][j];
				stat[p][j] = tmp;
			}
		}
		assert(b == num);
#ifdef TIMERS
		t.Stop();
		COUTF("Think part", t.ElapsedMicrosec());

		t.Start();
#endif
		if (dst_arr == NULL)
			dst_arr = test0;

		{
			std::thread threads[NUM_THREADS];
			for (size_t i = 0; i < NUM_THREADS - 1; i++)
				threads[i] = std::thread(work_thread_f, i, src_arr, dst_arr, num, stat[i], l);
			work_thread_f(NUM_THREADS - 1, src_arr, dst_arr, num, stat[NUM_THREADS - 1], l);
			for (size_t i = 0; i < NUM_THREADS - 1; i++)
				threads[i].join();
		}
#ifdef TIMERS
		t.Stop();
		COUTF("Work part", t.ElapsedMicrosec(), t.Mrps(num));
#endif

		std::swap(src_arr, dst_arr);
	}

#ifdef TIMERS
	t.Start();
#endif
	if (src_arr != arg_arr) {
		{
			auto thread_f = [&](size_t n) {
				size_t i1 = n * num / NUM_THREADS;
				size_t i2 = (n + 1) * num / NUM_THREADS;
				for (size_t i = i1; i < i2; i++) {
					dst_arr[i] = src_arr[i];
				}
			};
			std::thread threads[NUM_THREADS];
			for (size_t i = 0; i < NUM_THREADS - 1; i++)
				threads[i] = std::thread(thread_f, i);
			thread_f(NUM_THREADS - 1);
			for (size_t i = 0; i < NUM_THREADS - 1; i++)
				threads[i].join();
		}

		std::swap(src_arr, dst_arr);
	}
#ifdef TIMERS
	t.Stop();
	COUTF("Post part", t.ElapsedMicrosec());
#endif

	//free(dst_arr);

}

void __attribute__((noinline)) copy_test(Node* dst, Node* src, size_t count)
{
	memcpy(dst, src, count * sizeof(*dst));
}

void __attribute__((noinline)) strange_copy_test(Node* dst, Node* src, size_t count)
{
	for(Node *e = dst + count; dst < e; dst += 2, src += 2) {
		dst[1] = src[0];
		dst[0] = src[1];
	}
}

void __attribute__((noinline)) clear_test(Node* dst, size_t count)
{
	memset(dst, 0, count * sizeof(*dst));
}

struct TestTest {
	int i;
};

struct TestTest1 {
	alignas(16) int i;
};

int main()
{
	COUTF(alignof(TestTest), alignof(TestTest1));
	return 0;

	alya::CTimer t;

	t.Start();
	for (size_t i = 0; i < N; i++) {
		test[i].value = i;
		test[i].hint = rand();
		test[i].hint <<= 16;
		test[i].hint ^= rand();
		test[i].hint <<= 16;
		test[i].hint ^= rand();
		test[i].hint <<= 16;
		test[i].hint ^= rand();
		test[i].hint <<= 16;
		test[i].hint ^= rand();
		path[i].value = test[i].hint;
		path[i].orig_pos = i;
		test0[i] = test[i];
	}
	t.Stop();
	COUTF("Fill", t.Mrps(N));

	t.Start();
	strange_copy_test(test0, test, N);
	t.Stop();
	COUTF("Strange copy test", t.Mrps(N));

	t.Start();
	copy_test(test0, test, N);
	t.Stop();
	COUTF("Copy test", t.Mrps(N));

	t.Start();
	std::sort(path, path + N);
	t.Stop();
	COUTF("test sort", t.Mrps(N));

	t.Start();
	for (size_t i = 0; i < N; i++) {
		test[i] = test0[path[i].orig_pos];
	}
	t.Stop();
	COUTF("Reorder", t.Mrps(N));

	t.Start();
	clear_test(test0, N);
	t.Stop();
	COUTF("Clear test", t.Mrps(N));

	t.Start();
	static constexpr size_t STEP = 1024;
	for (size_t i = 0; i < N; i += STEP) {
		std::sort(test + i, test + i + STEP);
	}
	t.Stop();
	COUTF("Stupid sort", t.Mrps(N));

	t.Start();
	radix_mt(test, N);
//	std::sort(test, test + N);
	t.Stop();
	COUTF("Sort", t.Mrps(N));

	t.Start();
	for (size_t i = 0; i < N; i++) {
		if (i && test[i - 1].hint > test[i].hint)
			die();
		if (test[i].value >= N)
			die();
		uint64_t& m = mask[test[i].value / 64];
		uint64_t f = 1ull << (test[i].value % 64);
		if (m & f)
			die();
		m |= f;
	}
	t.Stop();
	COUTF("Test", t.Mrps(N));
}
