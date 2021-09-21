#include <alya.h>

#include <atomic>
#include <fstream>
#include <numeric>
#include <thread>
#include <vector>
#include <variant>
#include <cstring>

size_t n;
size_t a[100];

void read_params()
{
    std::fstream f("in.txt", std::ios::in);
    f >> n;
    COUTF(n);
    for (size_t i = 0; i < n; i++)
    {
        f >> a[i];
        std::cout << "a[" << i << "]=" << a[i] << " ";
    }
    std::cout << std::endl;
}

void test1()
{
    size_t res = 0;
    size_t count = 0;
    alya::CTimer t;
    t.Start();

    size_t w[100];
    for (size_t i = 0; i < n; i++)
    {
        w[i] = 0;
    }
    auto f = [&]()
    {
        count++;
        for (size_t i = 0; i < n; i++)
            res ^= w[i];
    };
    while (true)
    {
        f();
        bool more = false;
        for (size_t i = 0; i < n; i++)
        {
            if (++w[i] >= a[i])
            {
                w[i] = 0;
            }
            else
            {
                more = true;
                break;
            }
        }
        if (!more)
            break;
    }

    t.Stop();
    COUTF(t.ElapsedMicrosec(), count, res);
}

const size_t S = 16 * 1024 * 1024;
size_t arr[S];
size_t more[S];

void test_acc1()
{
    for (size_t i = 0; i < S; i++)
        arr[i] = rand();

    alya::CTimer t;
    t.Start();
    size_t res = 0;
    for (size_t i = 0; i < S; i++)
        res ^= arr[rand() % S];

    t.Stop();
    COUTF(t.Mrps(S), __func__, res);
}

void test_acc2()
{
    for (size_t i = 0; i < S; i++)
        arr[i] = rand();

    alya::CTimer t;
    t.Start();
    size_t res = 0;
    for (size_t i = 0; i < S; i++)
        more[i] = rand() % S;

    for (size_t i = 0; i < S; i++)
        res ^= arr[more[i]];

    t.Stop();
    COUTF(t.Mrps(S), __func__, res);
}

void test_acc3()
{
    for (size_t i = 0; i < S; i++)
        arr[i] = rand();

    alya::CTimer t;
    t.Start();
    size_t res = 0;
    for (size_t i = 0; i < S; i++)
        more[i] = rand() % S;
    t.Stop();
    COUTF(t.Mrps(S), __func__, res);

    t.Start();
    for (size_t i = 0; i < S; i++)
        res ^= arr[more[i]];

    t.Stop();
    COUTF(t.Mrps(S), __func__, res);
}

void test_acc4()
{
    for (size_t i = 0; i < S; i++)
        arr[i] = rand();

    alya::CTimer t;
    t.Start();
    size_t res = 0;
    for (size_t i = 0; i < S; i++)
        more[i] = rand() % S;
    t.Stop();
    COUTF(t.Mrps(S), __func__, res);

    t.Start();
    for (size_t i = 0; i < S; i += 128)
    {
        for (size_t j = i; j < i + 128; j++)
            __builtin_prefetch(&arr[more[j]], 0);
        for (size_t j = i; j < i + 128; j++)
            res ^= arr[more[j]];
    }
    t.Stop();
    COUTF(t.Mrps(S), __func__, res);
}

using my_key_t = std::vector<uint32_t>;

inline uint64_t murmurhash2_64(const void* key, const size_t len, const uint64_t seed = 0x27864C1E)
{
    const uint64_t m = 0xc6a4a7935bd1e995LLU;
    const int r = 47;

    uint64_t h = seed ^ (len * m);

    const uint64_t* data = (const uint64_t *)key;
    const uint64_t* end = data + (len/8);

    while (data != end)
    {
        uint64_t k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

//    const unsigned char* data2 = (const unsigned char*)data;
//
//    switch (len & 7)
//    {
//    case 7: h ^= uint64_t(data2[6]) << 48; [[fallthrough]];
//    case 6: h ^= uint64_t(data2[5]) << 40; [[fallthrough]];
//    case 5: h ^= uint64_t(data2[4]) << 32; [[fallthrough]];
//    case 4: h ^= uint64_t(data2[3]) << 24; [[fallthrough]];
//    case 3: h ^= uint64_t(data2[2]) << 16; [[fallthrough]];
//    case 2: h ^= uint64_t(data2[1]) << 8;  [[fallthrough]];
//    case 1: h ^= uint64_t(data2[0]);
//        h *= m;
//    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}
inline uint64_t mur(uint64_t* key, size_t count, const uint64_t seed = 0x27864C1E)
{
    //return murmurhash2_64_32((uint32_t*)(void*)&k, 2, seed);
    const uint64_t m = 0xc6a4a7935bd1e995LLU;
    const int r = 47;

    uint64_t h = seed ^ (count * sizeof(uint64_t) * m);

    for (size_t i = 0; i < count; i++)
    {
        uint64_t k = key[i];

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

size_t hash0(void* key, size_t count, uint64_t const seed)
{
    return murmurhash2_64(key, count, seed);
}

size_t hash0(uint64_t* key, size_t count, uint64_t const seed)
{
    return mur(key, count, seed);
}

uint32_t levels_num() {return 2; }

//__attribute__((noinline))
uint32_t hash32(bool const typed, uint32_t const type, uint32_t const level, void* key, size_t count)
{
    size_t const seed = typed ? type * levels_num() + level : level;
    return hash0(key, count, seed);
}

//__attribute__((noinline))
uint32_t hash64(bool const typed, uint32_t const type, uint32_t const level, uint64_t* key, size_t count)
{
    size_t const seed = typed ? type * levels_num() + level : level;
    return hash0(key, count, seed);
}

uint32_t k1_size = 7;
uint32_t k1[7];
uint32_t k2_size = 15;
uint32_t k2[15];

uint32_t get1()
{
    uint32_t res = 0;
    const uint32_t S = 1024 * 1024;
    my_key_t key(2);
    for (uint32_t i = 0; i < S; i++)
    {
        for (uint32_t j = 0; j < k1_size; j++)
        {
            key[0] = k1[j];
            for (uint32_t k = 0; k < k2_size; k++)
            {
                key[1] = k2[k];
                res ^= hash32(false, 0, i, key.data(), 8);
            }
        }
    }
    return res;
}

uint32_t get2()
{
    uint32_t res = 0;
    const uint32_t S = 1024 * 1024;
    my_key_t key(2);
    for (uint32_t i = 0; i < S; i++)
    {
        for (uint32_t j = 0; j < k1_size; j++)
        {
            key[0] = k1[j];
            for (uint32_t k = 0; k < k2_size; k++)
            {
                key[1] = k2[k];
                res ^= hash32(false, 0, i, key.data(), key.size() * sizeof(key[0]));
            }
        }
    }
    return res;
}

void test_hash1()
{
    alya::CTimer t;
    t.Start();
    uint32_t res = get2();
    t.Stop();
    COUTF(t.Mrps(S), __func__, res);
}

volatile size_t ONE = 1;

void test_hash2()
{
    alya::CTimer t;
    t.Start();
    uint32_t res = 0;
    const uint32_t S = 1024 * 1024;
    for (uint32_t i = 0; i < S; i++)
    {
        for (uint32_t j = 0; j < k1_size; j++)
        {
            uint64_t key = k1[j];
            for (uint32_t k = 0; k < k2_size; k++)
            {
                key &= UINT32_MAX;
                key |= uint64_t(k2[k]) << 32;
                res ^= hash64(false, 0, i, &key, ONE);
            }
        }
    }
    t.Stop();
    COUTF(t.Mrps(S), __func__, res);
}

int main()
{
    read_params();
    test1();
    test_acc1();
    test_acc2();
    test_acc3();
    test_acc4();

    for (uint32_t i = 0; i < k1_size; i++)
        k1[i] = rand();
    for (uint32_t i = 0; i < k2_size; i++)
        k2[i] = rand();

    test_hash1();
    test_hash2();
}
