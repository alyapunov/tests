#include <alya.h>

#include <climits>
#include <cassert>
#include <iostream>
#include <tuple>
#include <cstring>
#include <math.h>
#include <iterator>
#include <algorithm>
#include <set>

template <class T>
void print(T* arr, size_t size)
{
	std::copy(arr, arr + size, std::ostream_iterator<T>(std::cout, " "));
	std::cout << std::endl;
}


struct var {
	uint8_t type;
	union {
		int int_val;
		unsigned int uint_val;
		long long_val;
		unsigned long ulong_val;
		float flt_val;
		double dbl_val;
	};
	void set(int v) { type = 0; int_val = v; }
	void set(unsigned int v) { type = 1; uint_val = v; }
	void set(long v) { type = 2; long_val = v; }
	void set(unsigned long v) { type = 3; ulong_val = v; }
	void set(float v) { type = 4; flt_val = v; }
	void set(double v) { type = 5; dbl_val = v; }
	template <class VISITOR>
	auto visit1(VISITOR&& vis)
	{
		switch(type) {
			case 0: return vis(int_val);
			case 1: return vis(uint_val);
			case 2: return vis(long_val);
			case 3: return vis(ulong_val);
			case 4: return vis(flt_val);
			case 5: return vis(dbl_val);
			default : __builtin_unreachable();
		};
	}
	template <class F>
	auto vis0(F&& f) { return f(int_val); }
	template <class F>
	auto vis1(F&& f) { return f(uint_val); }
	template <class F>
	auto vis2(F&& f) { return f(long_val); }
	template <class F>
	auto vis3(F&& f) { return f(ulong_val); }
	template <class F>
	auto vis4(F&& f) { return f(flt_val); }
	template <class F>
	auto vis5(F&& f) { return f(dbl_val); }
	template <class F>
	auto visit2(F&& f)
	{
		using ptr_t = decltype(f(0)) (var::*)(F&&);
		static ptr_t p[6] = {
			&var::vis0<F &&>,
			&var::vis1<F &&>,
			&var::vis2<F &&>,
			&var::vis3<F &&>,
			&var::vis4<F &&>,
			&var::vis5<F &&>
		};
		ptr_t x = p[type];
		return (this ->* x)(std::forward<F>(f));
	}

#if 0
	template <class F>
	auto visit3(F&& f)
	{
		static void *l[6] = {&&L0, &&L1, &&L2, &&L3, &&L4, &&L5};
		goto *l[type];
		L0: return f(int_val);
		L1: return f(uint_val);
		L2: return f(long_val);
		L3: return f(ulong_val);
		L4: return f(flt_val);
		L5: return f(dbl_val);
	}
#endif
};

constexpr size_t N = 16 * 1024 * 1024;
var v[N];

using res_t = std::tuple<int, unsigned, long, unsigned long, float, double>;

res_t __attribute__((noinline)) test1(var* p, size_t n) {
	res_t res{};
	auto f = [&res](auto t) { std::get<decltype(t)>(res) += t; };
	for (var* e = p + n; p < e; p++) {
		p->visit1(f);
	}
	return res;
}

res_t __attribute__((noinline)) test2(var* p, size_t n) {
	res_t res{};
	auto f = [&res](auto t) { std::get<decltype(t)>(res) += t; };
	for (var* e = p + n; p < e; p++) {
		p->visit2(f);
	}
	return res;
}

#if 0
res_t __attribute__((noinline)) test3(var* p, size_t n) {
	res_t res{};
	auto f = [&res](auto t) { std::get<decltype(t)>(res) += t; };
	for (var* e = p + n; p < e; p++) {
		p->visit3(f);
	}
	return res;
}
#endif

void fill()
{
	srand(0);
	for (size_t i = 0; i < N; i++) {
		int r = rand() % 6;
		int s = rand() % 10;
		switch (r) {
			case 0: v[i].set((int)s); break;
			case 1: v[i].set((unsigned)s); break;
			case 2: v[i].set((long)s); break;
			case 3: v[i].set((unsigned long)s); break;
			case 4: v[i].set((float)s); break;
			case 5: v[i].set((double)s); break;
			default: __builtin_unreachable();
		};
	}
}

void prn(res_t r)
{
	COUTF(std::get<0>(r),
	      std::get<1>(r),
	      std::get<2>(r),
	      std::get<3>(r),
	      std::get<4>(r),
	      std::get<5>(r));
}

using action_t = size_t(*)();
const action_t *actions;

template <uint8_t I>
size_t action();

template <uint8_t I>
const action_t *get_actions()
{
#define A(x, y) &action<uint8_t(I+x*8+y)>
	constexpr static action_t res[256] =
	{
		A( 0, 0), A( 0, 1), A( 0, 2), A( 0, 3), A( 0, 4), A( 0, 5), A( 0, 6), A( 0, 7),
		A( 1, 0), A( 1, 1), A( 1, 2), A( 1, 3), A( 1, 4), A( 1, 5), A( 1, 6), A( 1, 7),
		A( 2, 0), A( 2, 1), A( 2, 2), A( 2, 3), A( 2, 4), A( 2, 5), A( 2, 6), A( 2, 7),
		A( 3, 0), A( 3, 1), A( 3, 2), A( 3, 3), A( 3, 4), A( 3, 5), A( 3, 6), A( 3, 7),
		A( 4, 0), A( 4, 1), A( 4, 2), A( 4, 3), A( 4, 4), A( 4, 5), A( 4, 6), A( 4, 7),
		A( 5, 0), A( 5, 1), A( 5, 2), A( 5, 3), A( 5, 4), A( 5, 5), A( 5, 6), A( 5, 7),
		A( 6, 0), A( 6, 1), A( 6, 2), A( 6, 3), A( 6, 4), A( 6, 5), A( 6, 6), A( 6, 7),
		A( 7, 0), A( 7, 1), A( 7, 2), A( 7, 3), A( 7, 4), A( 7, 5), A( 7, 6), A( 7, 7),
		A( 8, 0), A( 8, 1), A( 8, 2), A( 8, 3), A( 8, 4), A( 8, 5), A( 8, 6), A( 8, 7),
		A( 9, 0), A( 9, 1), A( 9, 2), A( 9, 3), A( 9, 4), A( 9, 5), A( 9, 6), A( 9, 7),
		A(10, 0), A(10, 1), A(10, 2), A(10, 3), A(10, 4), A(10, 5), A(10, 6), A(10, 7),
		A(11, 0), A(11, 1), A(11, 2), A(11, 3), A(11, 4), A(11, 5), A(11, 6), A(11, 7),
		A(12, 0), A(12, 1), A(12, 2), A(12, 3), A(12, 4), A(12, 5), A(12, 6), A(12, 7),
		A(13, 0), A(13, 1), A(13, 2), A(13, 3), A(13, 4), A(13, 5), A(13, 6), A(13, 7),
		A(14, 0), A(14, 1), A(14, 2), A(14, 3), A(14, 4), A(14, 5), A(14, 6), A(14, 7),
		A(15, 0), A(15, 1), A(15, 2), A(15, 3), A(15, 4), A(15, 5), A(15, 6), A(15, 7),

		/*
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0), A( 0, 0),
		 */

		A(16, 0), A(16, 1), A(16, 2), A(16, 3), A(16, 4), A(16, 5), A(16, 6), A(16, 7),
		A(17, 0), A(17, 1), A(17, 2), A(17, 3), A(17, 4), A(17, 5), A(17, 6), A(17, 7),
		A(18, 0), A(18, 1), A(18, 2), A(18, 3), A(18, 4), A(18, 5), A(18, 6), A(18, 7),
		A(19, 0), A(19, 1), A(19, 2), A(19, 3), A(19, 4), A(19, 5), A(19, 6), A(19, 7),
		A(20, 0), A(20, 1), A(20, 2), A(20, 3), A(20, 4), A(20, 5), A(20, 6), A(20, 7),
		A(21, 0), A(21, 1), A(21, 2), A(21, 3), A(21, 4), A(21, 5), A(21, 6), A(21, 7),
		A(22, 0), A(22, 1), A(22, 2), A(22, 3), A(22, 4), A(22, 5), A(22, 6), A(22, 7),
		A(23, 0), A(23, 1), A(23, 2), A(23, 3), A(23, 4), A(23, 5), A(23, 6), A(23, 7),
		A(24, 0), A(24, 1), A(24, 2), A(24, 3), A(24, 4), A(24, 5), A(24, 6), A(24, 7),
		A(25, 0), A(25, 1), A(25, 2), A(25, 3), A(25, 4), A(25, 5), A(25, 6), A(25, 7),
		A(26, 0), A(26, 1), A(26, 2), A(26, 3), A(26, 4), A(26, 5), A(26, 6), A(26, 7),
		A(27, 0), A(27, 1), A(27, 2), A(27, 3), A(27, 4), A(27, 5), A(27, 6), A(27, 7),
		A(28, 0), A(28, 1), A(28, 2), A(28, 3), A(28, 4), A(28, 5), A(28, 6), A(28, 7),
		A(29, 0), A(29, 1), A(29, 2), A(29, 3), A(29, 4), A(29, 5), A(29, 6), A(29, 7),
		A(30, 0), A(30, 1), A(30, 2), A(30, 3), A(30, 4), A(30, 5), A(30, 6), A(30, 7),
		A(31, 0), A(31, 1), A(31, 2), A(31, 3), A(31, 4), A(31, 5), A(31, 6), A(31, 7),
	};
	return res;
}

static size_t mistique = 0;

template <uint8_t I>
size_t action()
{
	actions = get_actions<I>();
	mistique += size_t(I) * size_t(I) * size_t(I) * size_t(I);
	size_t more = 1;
	//for (size_t i = 0; i < 16; i++) {
	for (size_t i = 0; i < 2; i++) {
		more += ((mistique >> i) & 1) * I;
	}
	mistique += more;
	return mistique;
}



size_t __attribute__((noinline)) thetest(const char* p, size_t n)
{
	size_t res = 0;
	for (const char *e = p + n; p < e; ++p) {
		uint8_t i = *p;
		res ^= actions[i]();
	}
	return res;
}


template <char... C>
struct Str {
	static constexpr size_t size = sizeof...(C);
	static constexpr char data[size + 1] = {C...};
	template <char... D>
	static constexpr Str<C..., D...> join(Str<D...>) { return {}; }
	template <size_t... I>
	static constexpr Str<data[I]...> subs(std::index_sequence<I...>) { return {}; }
};

template <class T, class SEQ> struct ToStrHlp;

template <class T, size_t... I>
struct ToStrHlp<T, std::index_sequence<I...>> {
	using type = Str<((T::value >> (8 * (sizeof...(I)-I-1))) & 0xff)...>;
};

template <class T>
using ToStr = typename ToStrHlp<T, std::make_index_sequence<sizeof(T::value)>>::type;

template <class STR, class SEQ>
struct SubStrHlp;

template <class STR, size_t... I>
struct SubStrHlp<STR, std::index_sequence<I...>> {
	using type = Str<STR::data[I]...>;
};

template <class STR, size_t N>
using SubStr = typename SubStrHlp<STR, std::make_index_sequence<N>>::type;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

namespace literal {
template <class T, T... C>
constexpr Str<C...> operator""_cs() {
	static_assert(std::is_same_v<char, T>, "Only for char type");
	return {  };
}
} // namespace literal {

#ifdef __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

#define MPP_CON_STR_HLP0(S, N) S[(N) % std::size(S)]
#define MPP_CON_STR_HLP1(S, N) MPP_CON_STR_HLP0(S, N), MPP_CON_STR_HLP0(S, N+ 1)
#define MPP_CON_STR_HLP2(S, N) MPP_CON_STR_HLP1(S, N), MPP_CON_STR_HLP1(S, N+ 2)
#define MPP_CON_STR_HLP3(S, N) MPP_CON_STR_HLP2(S, N), MPP_CON_STR_HLP2(S, N+ 4)
#define MPP_CON_STR_HLP4(S, N) MPP_CON_STR_HLP3(S, N), MPP_CON_STR_HLP3(S, N+ 8)
#define MPP_CON_STR_HLP5(S, N) MPP_CON_STR_HLP4(S, N), MPP_CON_STR_HLP4(S, N+16)
#define MPP_CON_STR_HLP6(S, N) MPP_CON_STR_HLP5(S, N), MPP_CON_STR_HLP5(S, N+32)

template <size_t S>
struct MppMacroIndexes {
	static_assert(S <= 64);
	using IndSeq_t = std::make_index_sequence<S>;
};
#define MPP_CON_STR_IS(S) MppMacroIndexes<std::size(S) - 1>::IndSeq_t{}
#define MPP_CON_STR(S) Str<MPP_CON_STR_HLP6(S, 0)>::subs(MPP_CON_STR_IS(S))


template <class... T>
char *set1(char* dst, T...)
{
	(..., memcpy((dst += sizeof(T::value)) - sizeof(T::value),
		     &T::value, sizeof(T::value)));
	return dst;
}

template <char... C>
char *set2(char *dst, Str<C...>)
{
	memcpy(dst, Str<C...>::data, Str<C...>::size);
	return dst += Str<C...>::size;
}

template <char... C, char... D, class... T>
char *set2(char *dst, Str<C...>, Str<D...>, T... t)
{
	return set2(dst, Str<C..., D...>{}, t...);
}

template <class... T>
char *set2(char *dst, T...)
{
	return set2(dst, ToStr<T>{}...);
}

template <char... C>
char *set3(char *dst, Str<C...>)
{
	memcpy(dst, Str<C...>::data, 8);
	memcpy(dst + 5, Str<C...>::data+5, 8);
	return dst += Str<C...>::size;
}

template <char... C, char... D, class... T>
char *set3(char *dst, Str<C...>, Str<D...>, T... t)
{
	return set3(dst, Str<C..., D...>{}, t...);
}

template <class... T>
char *set3(char *dst, T...)
{
	return set3(dst, ToStr<T>{}...);
}


__attribute__((noinline)) char *testset1(char *dst)
{
	return set1(dst
		, std::integral_constant<uint32_t, 0x04030201>{}
		, std::integral_constant<uint8_t, 0x05>{}
		, std::integral_constant<uint16_t, 0x0706>{}
		, std::integral_constant<uint16_t, 0x0908>{}
		, std::integral_constant<uint16_t, 0x0B0A>{}
		, std::integral_constant<uint16_t, 0x0D0C>{}
	);
}

__attribute__((noinline)) char *testset2(char *dst)
{
	return set2(dst
		, std::integral_constant<uint32_t, 0x04030201>{}
		, std::integral_constant<uint8_t, 0x05>{}
		, std::integral_constant<uint16_t, 0x0706>{}
		, std::integral_constant<uint16_t, 0x0908>{}
		, std::integral_constant<uint16_t, 0x0B0A>{}
		, std::integral_constant<uint16_t, 0x0D0C>{}
	);
}

__attribute__((noinline)) char *testset3(char *dst)
{
	return set3(dst
		, std::integral_constant<uint32_t, 0x04030201>{}
		, std::integral_constant<uint8_t, 0x05>{}
		, std::integral_constant<uint16_t, 0x0706>{}
		, std::integral_constant<uint16_t, 0x0908>{}
		, std::integral_constant<uint16_t, 0x0B0A>{}
		, std::integral_constant<uint16_t, 0x0D0C>{}
	);
}

__attribute__((noinline)) char *testset4(char *dst)
{
	static constexpr struct __attribute__((packed)) {
		uint32_t a = 0x04030201;
		uint8_t b = 0x05;
		uint16_t c = 0x0706;
		uint16_t d = 0x0908;
		uint16_t e = 0x0B0A;
		uint16_t f = 0x0D0C;
	} data;
	memcpy(dst, &data, sizeof(data));
	return dst += sizeof(data);
}

char testdata1[N];
char testdata2[N];
char testdata3[N];
char testdata4[N];



template <class T, class U>
auto extract(const T& t, const U& u)
{
	if constexpr (std::is_member_object_pointer_v<U>)
		return t.*u;
	else
		return u;
}

struct Ring {
	Ring* ptrs[2];
};

struct alignas(16) ListLink : Ring {
};

struct alignas(32) ListHead {
	void *padding1;
	Ring ring;
	void *padding2;
};

static_assert(sizeof(ListLink) == 16);
static_assert(sizeof(ListHead) == 32);




template <size_t I, class T, size_t N>
constexpr const T& get(const T (&a)[N]) noexcept
{
	return a[I];
}

template <size_t I, class... T>
const typename std::tuple_element<I, const std::tuple<T...>>::type&
get(const std::tuple<T...>& t) noexcept
{
	return std::get<I>(t);
}

template <size_t I, class T>
decltype(auto)
get(const T& t) noexcept
{
	return t.data()[I];
}



void test() {}

template <class T>
void test(const T& t)
{
	if constexpr (T::is_static) {
		if constexpr (!t.has()) {
			return test();
		} else {
			COUTF(t.get());
			return test(t.next());
		}
	} else {
		if (!t.has()) {
			return test();
		} else {
			COUTF(t.get());
			return test(t.next());
		}
	}
}

void prn()
{
	std::cout << std::endl;
}

template <class T, class... U>
void prn(const T& t, const U&... u)
{
	std::cout << t << " ";
	prn(u...);
}

template <class T, size_t... I>
struct unpacker { const T& value; };

template <class T, size_t... I>
auto get_unpacker(const T& t, std::index_sequence<I...>)
{
	return unpacker<T, I...>{t};
}

template <class... T>
auto unpack(const std::tuple<T...>& t)
{
	return get_unpacker(t, std::make_index_sequence<sizeof...(T)>{});
}

template <class T, size_t N>
auto unpack(const std::array<T, N>& t)
{
	return get_unpacker(t, std::make_index_sequence<N>{});
}

template <class T, class U, size_t... I, class V>
void test(const T&t, const unpacker<U, I...>& u, const V& v)
{
	prn(t, std::get<I>(u.value)..., v);
}

template <class T, class U, size_t N, class V>
void test(const T&t, const std::array<U, N>& u, const V& v)
{
	test(t, unpack(u), v);
}

template <class T, class... U, class V>
void test(const T&t, const std::tuple<U...>& u, const V& v)
{
	test(t, unpack(u), v);
}


template <class T>
void f();
template <class T>
void g();

template <class T>
void f()
{
	if (std::is_integral_v<T>)
		return g<T>();
	std::cout << 10 * sizeof(T) << std::endl;
}

template <class T>
void g()
{
	if (std::is_floating_point_v<T>)
		return f<T>();
	std::cout << sizeof(T) << std::endl;
}



int main()
{
	aaa();
	test(1, std::make_tuple(2, "a"), 5);
	test("aaa", std::array<int, 5>{1, 2, 3, 4, 5}, "bbb");


//	int tarr[8] = {1, 2, 3, 4, 5};
//	const int carr[8] = {1, 2, 3, 4, 5};
//	std::array<int, 5> arr = {0, 1, 2, 3, 4};
	//auto tup = std::make_tuple(12, 3.14, "abc");
	std::vector<int> vec = {1, 2, 3, 4, 5};
	std::set<int> set = {1, 2, 3, 4, 5};
	std::string str = "abcde";

//	test(get_extractor(tarr));
//	test(get_extractor(carr));
//	test(get_extractor(arr));
//	test(get_extractor(std::forward_as_tuple(12, 3.5, "test")));
//	test(get_extractor(vec));
	return 0;

	#if 0

	TESTRANGE(range(tarr, tarr + 3));
	TESTRANGE(range(tarr, 4));
	TESTRANGE(range(tarr + 1, tarr + 3));
	TESTRANGE(range(tarr + 1, 4));
	TESTRANGE(range<2>(tarr));

	TESTRANGE(range(carr, carr + 3));
	TESTRANGE(range(carr, 4));
	TESTRANGE(range(carr + 1, carr + 3));
	TESTRANGE(range(carr + 1, 4));

	TESTRANGE(range<2>(carr));

	TESTRANGE(range(std::begin(arr), std::begin(arr) + 3));
	TESTRANGE(range(std::begin(arr), 4));
	TESTRANGE(range(std::begin(arr) + 1, std::begin(arr) + 3));
	TESTRANGE(range(std::begin(arr) + 1, 4));
	TESTRANGE(range<2>(std::begin(arr)));

	TESTRANGE(range(std::begin(vec), std::begin(vec) + 3));
	TESTRANGE(range(std::begin(vec), 4));
	TESTRANGE(range(std::data(vec), 4));
	TESTRANGE(range(std::begin(vec) + 1, std::begin(vec) + 3));
	TESTRANGE(range(std::begin(vec) + 1, 4));
	TESTRANGE(range(std::data(vec) + 1, 4));
	TESTRANGE(range<2>(std::begin(vec)));

	TESTRANGE(range(std::begin(set), 4));
	TESTRANGE(range<2>(std::begin(set)));

	#endif

	return 0;

	ListLink ll;
	ListHead lh;
	COUTF(&ll, &lh.ring);

	COUTF(sizeof(timespec::tv_nsec));

	constexpr size_t K = 32;
	size_t MaxMrps[4] = {};
	double side = 0;
	size_t Mrps[4];
#define RUN(id)									\
	do {									\
		alya::CTimer t;							\
		t.Start();							\
		char *p;							\
		char *e = testdata##id + N - 11;				\
		for (size_t k = 0; k < K; k++) {				\
			p = testdata##id;					\
			while (p < e)						\
				p = testset##id(p);				\
		}								\
		t.Stop();							\
		Mrps[id - 1] = t.Mrps(N * K);					\
		side += (p - e) * pow10(id);					\
	} while (0)
	for (size_t i = 0; i < 20; i++) {
		RUN(1);
		RUN(2);
		RUN(3);
		RUN(4);
		for (size_t j = 0; j < 4; j++)
			MaxMrps[j] = std::max(MaxMrps[j], Mrps[j]);
	}
	COUTF(MaxMrps[0], MaxMrps[1], MaxMrps[2], MaxMrps[3], side); side = 0;

	for (size_t i = 0; i < 25; i++)
		std::cout << int((uint8_t)testdata1[i]) << " ";
	std::cout << "\n";
	for (size_t i = 0; i < N; i++) {
		if (testdata1[i] != testdata2[i])
			throw 1;
		if (testdata1[i] != testdata3[i])
			throw 2;
		if (testdata1[i] != testdata4[i])
			throw 2;
	}



	return 0;



	/*
	COUT("template <size_t I> void f();");
	NL;
	COUT("void main(int n, const char**) {");
	COUT("\tswitch(n) {");
	for (size_t i = 0; i < 256; i++)
		COUT("\t\tcase ", i, ": return f<", i, ">();");
	COUT("\t\tdefault: __builtin_unreachable();");
	COUT("\t}");
	COUT("}");
	 */

	{
		actions = get_actions<0>();
		char data[N];
		srand(0);
		for (char &x : data) x = rand();
		alya::CTimer t;
		t.Start();
		size_t side_effect = thetest(data, N);
		t.Stop();
		COUTF(t.Mrps(N), side_effect, "jumps");
	}
	{
		actions = get_actions<0>();
		char data[N];
		srand(0);
		for (char &x : data) x = rand();
		alya::CTimer t;
		t.Start();
		size_t side_effect = thetest(data, N);
		t.Stop();
		COUTF(t.Mrps(N), side_effect, "jumps");
	}


	{
		fill();
		alya::CTimer t;
		t.Start();
		auto r = test1(v, N);
		t.Stop();
		COUTF(t.Mrps(N));
		prn(r);
	}
	{
		fill();
		alya::CTimer t;
		t.Start();
		auto r = test2(v, N);
		t.Stop();
		COUTF(t.Mrps(N));
		prn(r);
	}
#if 0
	{
		fill();
		alya::CTimer t;
		t.Start();
		auto r = test3(v, N);
		t.Stop();
		COUTF(t.Mrps(N));
		prn(r);
	}
#endif

}
