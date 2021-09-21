#include <alya.h>
#include <stdio.h>

#include <tuple>

struct A {
	static int NewID()
	{
		static int id = 0; return id++;

	}

	int m_A_id;
	int *m_p;
	int id() const { return m_A_id; }

	A(int *p) : m_A_id(NewID()), m_p(p)
	{
		std::cout << "A" << "(" << m_A_id << ") ctor" << std::endl;
	}
	~A()
	{
		std::cout << "A" << "(" << m_A_id << ") dtor" << std::endl;
	}
	A(const A& a) : m_A_id(NewID()), m_p(a.m_p)
	{
		std::cout << "A" << "(" << m_A_id << ") copy from (" << a.m_A_id << ") ctor" << std::endl;
	}
	A(A &&a) : m_A_id(NewID()), m_p(a.m_p)
	{
		std::cout << "A" << "(" << m_A_id << ") move from (" << a.m_A_id << ") ctor" << std::endl;
	}
	const A& operator=(const A& a)
	{
		m_p = a.m_p;
		std::cout << "A" << "(" << m_A_id << ") assign from (" << a.m_A_id << ") ctor" << std::endl;
		return *this;
	}
	A& operator=(A&& a)
	{
		m_p = a.m_p;
		std::cout << "A" << "(" << m_A_id << ") move assign from (" << a.m_A_id << ") ctor" << std::endl;
		return *this;
	}
	int& operator*() const { return *m_p; }
	void operator++() { ++m_p; }
	bool operator<(const A& a) const { return m_p < a.m_p; }
	bool operator==(const A& a) const { return m_p == a.m_p; }
	bool operator!=(const A& a) const { return m_p != a.m_p; }
};

template <class>
constexpr bool always_false_v = false;

template <class ITR>
struct Range {
	ITR m_begin;
	ITR m_end;
	ITR& begin() { return m_begin; }
	ITR& end() { return m_end; }
};

template <class ITR>
Range<std::remove_reference_t<ITR>> range(ITR&& begin, ITR&& end)
{
	return {std::forward<ITR>(begin), std::forward<ITR>(end)};
}





struct Data {
	int i;
	float f;
	static constexpr auto mpp_rule = std::make_tuple(1, &Data::i);
	static constexpr auto mpp_enc_rule = std::make_tuple(1, &Data::i, 2, &Data::f);
};

/*
template <>
struct mpp_rule<Data> {
	static constexpr auto value = std::make_tuple();
};
*/

template <class T>
struct S {
	using value_type = T;
	using const_value_type = const T;
};

template <class T>
struct T1 {
	static constexpr const char* name = "T1";
};

template <class T>
struct T2 {
	static constexpr const char* name = "T2";
};

template <class T>
struct T3 {
	static constexpr const char* name = "T3";
};


namespace tnt {
template <class T>
auto subst(T1<T> t) {
	COUTS("tnt", T1<T>::name);
	return t;
}
}

namespace mpp {
template <class T>
auto subst(T2<T> t) {
	COUTS("mpp", T2<T>::name);
	return t;
}
}

namespace tnt {
using mpp::subst;
}

namespace tnt {
template <class T>
auto subst(T&& t) {
	COUTS("General", std::remove_reference_t<T>::name);
	return std::forward<T>(t);
}
}

TRACE_CLASS(AAA);

struct bf {
    bool b0 : 1;
    bool b1 : 1;
    bool b2 : 1;
    bool b3 : 1;
    bool b4 : 1;
    bool b5 : 1;
    bool b6 : 1;
    bool b7 : 1;
    bool b8 : 1;
};

double my_rand()
{
	return double(rand()) / RAND_MAX;
}

#include <math.h>

int main()
{
	uint32_t idata = 1;
	char strdata[4];
	memcpy(strdata, &idata, 4);
	COUTF(int(strdata[0]));
	COUTF(int(strdata[3]));
	return 0;


	for (int i = 0; i < 10000; i++) {
		double p = rand() % 100 + 1;

		double x = pow((my_rand() - 0.5)*2*my_rand(), p);
		char str[256];
		sprintf(str, "%.15lf", x);
		double y;
		sscanf(str, "%lg", &y);
		if (x != y)
			COUTF(x, y, str, x-y);
	}

	COUTF(BIN(-10));
	COUTF(BIN(246));
	COUTF(sizeof(long double));
	return 0;

	int jmp;
	goto a;
	jmp = 3;
a:
	COUTF(jmp);
	COUTF(sizeof(bf));
	return 0;

	auto ttt1 = std::make_tuple(AAA{});
	COUT("hi1");
	auto ttt3 = std::forward_as_tuple(AAA{});
	COUT("hi3");
	(void)ttt3;

	T1<int> t1;
	T2<int> t2;
	T3<int> t3;
	tnt::subst(t1);
	tnt::subst(t2);
	tnt::subst(t3);



	using SS = S<const int>;
	std::add_const_t<const int> x = 3;
	COUTF(x);
	COUTF(sizeof(SS::const_value_type));

//	int arr[] = {1, 2, 3};
//
//	for (int x : range(A(arr + 0), A(arr + 3)))
//		COUTF(x);
}
