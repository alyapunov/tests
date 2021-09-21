#include <set>

#include <benchmark/benchmark.h>
#include <boost/container/flat_set.hpp>

enum class Pos
{
	Begin,
	Middle,
	End,
	NotExist
};

template <class T>
struct TestCont {
	using key_type = T;
	static constexpr size_t SHARDS = 32;
	std::vector<T> shards[SHARDS];
	void insert(const T& t)
	{
		shards[t % SHARDS].push_back(t);
	}
	size_t count(const T& t) const
	{
		for (T x : shards[t % SHARDS])
			if (x == t)
				return 1;
		return 0;
	}
};

template <class T>
struct TestCont1 {
	using key_type = T;
	static constexpr size_t SHARDS = 32;
	boost::container::flat_set<T> shards[SHARDS];
	void insert(const T& t)
	{
		shards[t % SHARDS].insert(t);
	}
	size_t count(const T& t) const
	{
		return shards[t % SHARDS].count(t);
	}
};


template <class Cont, Pos tPos>
static void BM_SearchTest(benchmark::State& state)
{
	Cont m_Cont;
	for (int i = 0; i < state.range(0); ++i)
		m_Cont.insert(i);
	typename Cont::key_type middle;
	if constexpr (Pos::Begin == tPos)
		middle = 0;
	else if constexpr (Pos::Middle == tPos)
		middle = state.range(0) * 3 / 4;
	else if constexpr (Pos::End == tPos)
		middle = state.range(0) - 1;
	else
		middle = state.range(0) * 2;
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(m_Cont.count(middle));
	}
	state.SetItemsProcessed(state.iterations());
}

using T = int;
BENCHMARK_TEMPLATE(BM_SearchTest, std::set<T>, Pos::Begin)->Range(4, 1024);
BENCHMARK_TEMPLATE(BM_SearchTest, TestCont<T>, Pos::Begin)->Range(4, 1024);
BENCHMARK_TEMPLATE(BM_SearchTest, boost::container::flat_set<T>, Pos::Begin)->Range(4, 1024);
BENCHMARK_TEMPLATE(BM_SearchTest, std::set<T>, Pos::Middle)->Range(4, 1024);
BENCHMARK_TEMPLATE(BM_SearchTest, TestCont<T>, Pos::Middle)->Range(4, 1024);
BENCHMARK_TEMPLATE(BM_SearchTest, boost::container::flat_set<T>, Pos::Middle)->Range(4, 1024);
BENCHMARK_TEMPLATE(BM_SearchTest, std::set<T>, Pos::End)->Range(4, 1024);
BENCHMARK_TEMPLATE(BM_SearchTest, TestCont<T>, Pos::End)->Range(4, 1024);
BENCHMARK_TEMPLATE(BM_SearchTest, boost::container::flat_set<T>, Pos::End)->Range(4, 1024);
BENCHMARK_TEMPLATE(BM_SearchTest, std::set<T>, Pos::NotExist)->Range(4, 1024);
BENCHMARK_TEMPLATE(BM_SearchTest, TestCont<T>, Pos::NotExist)->Range(4, 1024);
BENCHMARK_TEMPLATE(BM_SearchTest, boost::container::flat_set<T>, Pos::NotExist)->Range(4, 1024);

BENCHMARK_MAIN();
