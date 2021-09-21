#include <alya.h>

#include <string.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <thread>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>

//#include <boost/algorithm/string/case_conv.hpp>

template <class T>
struct is_const
{
    static constexpr bool value = std::is_const<typename std::remove_reference<T>::type>::value;
};


TRACE_CLASS(A);

template <class T>
void f(T&& x)
{
    COUTF(std::is_reference<T>::value);
    COUTF(std::is_rvalue_reference<T>::value);
    COUTF(is_const<T>::value);
    COUTF(std::is_reference<decltype(x)>::value);
    COUTF(std::is_rvalue_reference<decltype(x)>::value);
    COUTF(is_const<decltype(x)>::value);
}

template <class T>
void ff(T x)
{
    COUTF(std::is_reference<T>::value);
    COUTF(std::is_rvalue_reference<T>::value);
    COUTF(is_const<T>::value);
    COUTF(std::is_reference<decltype(x)>::value);
    COUTF(std::is_rvalue_reference<decltype(x)>::value);
    COUTF(is_const<decltype(x)>::value);
}

__attribute__((noinline)) void test(size_t c)
{
    std::string_view str ("Hello world\nHello world\nHello world\nHello world\nHello world\nHello world\nHello world\nHello world\nHello world\nHello world\n");
    for (size_t i = 0; i < c; i++)
        std::cout << str;
}

__attribute__((noinline)) uint64_t test_internal(uint64_t *arr, uint64_t size)
{
    uint64_t res = 0;
    for (size_t i = 0; i < size; i++)
        res += __builtin_ctzl(arr[i]);
    return res;
}

__attribute__((noinline)) uint64_t wrap_test(uint64_t *arr, uint64_t size)
{
    alya::CTimer t;
    t.Start();
    uint64_t res = test_internal(arr, size);
    t.Stop();
    COUTF(t.Mrps(size));
    return res;
}

constexpr size_t SIZE = 1024 * 1024;
uint64_t arr[SIZE];


void prn(uint64_t v)
{
    std::cout << "--------------------" << std::endl;
    for (size_t i = 0; i < 8; i++) {
        for (size_t j = 0; j < 8; j++) {
            std::cout << ((v & (1ull << (i * 8  + j))) ? "o " : ". ");
        }
        std::cout << std::endl;
    }
}

uint64_t move(uint64_t& v)
{
    uint64_t res = v;
    uint64_t t;
    t  = (v >> 1) & 0x7F7F7F7F7F7F7F7Full;
    t |= (v << 1) & 0xFEFEFEFEFEFEFEFEull;
    res |= t << 16;
    res |= t >> 16;
    t  = (v >> 2) & 0x3F3F3F3F3F3F3F3Full;
    t |= (v << 2) & 0xFCFCFCFCFCFCFCFCull;
    res |= t << 8;
    res |= t >> 8;
    return res;
}


int main()
{
    uint64_t v = 1ull << 36;
    prn(v);
    for (size_t i = 0; i < 6; i++) {
        v = move(v);
        prn(v);
    }


    return 0;


    for (size_t i = 0; i < SIZE; i++)
        arr[i] = rand() + 1;
    COUTF(wrap_test(arr, SIZE));
    return 0;

    srand(time(0));
    size_t M = 1000;
    size_t N = 100;
    size_t R = 0;
    for (size_t i = 0; i < M; i++)
    {
        if (size_t(rand()) >= size_t((M - N) * RAND_MAX / M))
            R++;
    }
    COUTF(R);
    return 0;


    std::ios::sync_with_stdio(false);
    test(100000);


    return 0;

    A a;
    const A& b = std::move(a);
    A&& c = std::move(a);
    COUTF(std::is_reference<A&&>::value);
    COUTF(std::is_rvalue_reference<A&&>::value);
    COUTF(std::is_reference<decltype(b)>::value);
    COUTF(std::is_rvalue_reference<decltype(b)>::value);
    COUTF(is_const<decltype(b)>::value);
    COUTF(std::is_reference<decltype(c)>::value);
    COUTF(std::is_rvalue_reference<decltype(c)>::value);
    COUTF(is_const<decltype(c)>::value);

    COUT("---val---");
    f(a);
    COUT("---ref---");
    f(b);
    COUT("---move---");
    f(std::move(c));

    COUT("---val---");
    ff(a);
    COUT("---ref---");
    ff(b);
    COUT("---move---");
    ff(std::move(c));

    return 0;
    const char rus[33][2][4] = 
    {
        {"а", "А"},
        {"б", "Б"},
        {"в", "В"},
        {"г", "Г"},
        {"д", "Д"},
        {"е", "Е"},
        {"ё", "Ё"},
        {"ж", "Ж"},
        {"з", "З"},
        {"и", "И"},
        {"й", "Й"},
        {"к", "К"},
        {"л", "Л"},
        {"м", "М"},
        {"н", "Н"},
        {"о", "О"},
        {"п", "П"},
        {"р", "Р"},
        {"с", "С"},
        {"т", "Т"},
        {"у", "У"},
        {"ф", "Ф"},
        {"х", "Х"},
        {"ц", "Ц"},
        {"ч", "Ч"},
        {"ш", "Ш"},
        {"щ", "Щ"},
        {"ъ", "Ъ"},
        {"ы", "Ы"},
        {"ь", "Ь"},
        {"э", "Э"},
        {"ю", "Ю"},
        {"я", "Я"}
    };
    for (size_t i = 0; i < 33; i++)
    {
        std::cout << rus[i][0] << rus[i][1];
        for (size_t j = 0; j < 2; j++)
        {
            std::cout << ": ";
            for (size_t k = 0; k < 4; k++)
            {
                if (rus[i][j][k] == 0)
                    break;
                int a = (int)(unsigned char)rus[i][j][k];
                std::cout << a << " ";
            }
        }
        std::cout << "\n";
    }

    /*
    COUTF(std::setlocale(LC_ALL, "ru_RU.UTF-8"));
    const char* base = "aAbBzZаАёЁйЙяЯ";
    wchar_t base_w[30];
    wchar_t low_w[30];
    char low[30];

    size_t len_w = mbstowcs(base_w, base, strlen(base));
    if (len_w == static_cast<size_t>(-1))
        COUTF(len_w);
    base_w[len_w] = 0;

    for (size_t i = 0; i < len_w; i++)
        low_w[i] = std::towlower(base_w[i]);
    low_w[len_w] = 0;

    size_t len = wcstombs(low, low_w, len_w);
    low[len] = 0;

    std::cout << base << std::endl;
    std::wcout << base_w << std::endl;
    std::wcout << low_w << std::endl;
    std::cout << low << std::endl;
    */
}
