#include <alya.h>
#include <fstream>
#include <algorithm>

const size_t N = 16 * 1024 * 1024;
const size_t M = 1 * 1024 * 1024;
const size_t K = 8;

size_t v[N+1];

using namespace alya;

void wr()
{
    CTimer t;
    t.Start();
    std::ofstream f("data.txt");
    for (size_t i = 0; i < M; i++)
        f << rand() << "\n";
    t.Stop();
    COUTF(__func__, t.Mrps(M));
}

void gen()
{
    CTimer t;
    t.Start();
    for (size_t i = 0; i < N; i++)
        v[i] = rand();
    std::sort(v, v + N);
    t.Stop();
    COUTF(__func__, t.Mrps(N));
}

void rd()
{
    CTimer t;
    t.Start();
    std::ifstream f("data.txt");
    size_t r = 0;
    size_t x = 0;
    for (size_t i = 0; i < M; i++)
    {
        f >> x;
        r ^= x;
    }
    t.Stop();
    COUTF(__func__, r, t.Mrps(M));
}

size_t* lower_bound(size_t x)
{
    //lower_bound lowest pos: !(v[pos] < x)

    size_t* beg = v;
    size_t* fin = v + N;
    do
    {
        size_t* mid = beg + (fin - beg) / 2;
        bool less = *mid < x;
#if 1
        if (less)
            beg = mid + 1;
        else
            fin = mid;
#else
        *(less ? &beg : &fin) = mid + less;
#endif
    } while (fin != beg);
    return beg;
}

void fnd()
{
    CTimer t;
    t.Start();
    std::ifstream f("data.txt");
    size_t r = 0;
    size_t x = 0;
    for (size_t i = 0; i < M; i++)
    {
        f >> x;
        r ^= *lower_bound(x);
    }
    t.Stop();
    COUTF(__func__, r, t.Mrps(M));
}

void fnd_opt1()
{
    CTimer t;
    t.Start();
    std::ifstream f("data.txt");
    size_t r = 0;
    size_t x[16];
    for (size_t i = 0; i < M; i += K)
    {
        for (size_t k = 0; k < K; k++)
            f >> x[k];
        for (size_t k = 0; k < K; k++)
            r ^= *lower_bound(x[k]);
    }
    t.Stop();
    COUTF(__func__, r, t.Mrps(M));
}

void lower_bound_burst(size_t* x, size_t** res)
{
    size_t* beg[K];
    size_t* fin[K];
    for (size_t k = 0; k < K; k++)
    {
        beg[k] = v;
        fin[k] = v + N;
    }
    bool cont;
    do
    {
        cont = false;
        for (size_t k = 0; k < K; k++)
        {
            if (beg[k] == fin[k])
                continue;
            cont = true;

            size_t* mid = beg[k] + (fin[k] - beg[k]) / 2;
            bool less = *mid < x[k];
            if (less)
                beg[k] = mid + 1;
            else
                fin[k] = mid;
            mid = beg[k] + (fin[k] - beg[k]) / 2;
            __builtin_prefetch(mid, 0);
        }
    } while (cont);
    for (size_t k = 0; k < K; k++)
        res[k] = beg[k];
}

void fnd_opt2()
{
    CTimer t;
    t.Start();
    std::ifstream f("data.txt");
    size_t r = 0;
    size_t x[16];
    size_t* res[16];
    for (size_t i = 0; i < M; i += K)
    {
        for (size_t k = 0; k < K; k++)
            f >> x[k];
        lower_bound_burst(x, res);
        for (size_t k = 0; k < K; k++)
            r ^= *res[k];
    }
    t.Stop();
    COUTF(__func__, r, t.Mrps(M));
}

void burst2_internal(size_t* haystack_begin,
                     size_t* haystack_end,
                     size_t* needle_begin,
                     size_t* needle_end,
                     size_t** res_begin,
                     size_t** res_end)
{
    if (haystack_begin == haystack_end)
    {
        do
        {
            *res_begin = haystack_end;
            res_begin++;
        } while (res_begin != res_end);
        return;
    }
    size_t* mid = haystack_begin + (haystack_end - haystack_begin) / 2;
    size_t* nmid = needle_begin;
    size_t** rmid = res_begin;
    while (nmid != needle_end && *nmid <= *mid)
    {
        nmid++;
        rmid++;
    }
    if (needle_begin != nmid)
    {
        if (haystack_begin != mid)
        {
            burst2_internal(haystack_begin, mid, needle_begin, nmid, res_begin, rmid);
        }
        else
        {
            size_t** r = res_begin;
            do {
                *r = mid;
                r++;
            } while (r != rmid);
        }
    }
    mid++;
    if (nmid != needle_end)
    {
        if (mid != haystack_end)
        {
            burst2_internal(mid, haystack_end, nmid, needle_end, rmid, res_end);
        }
        else
        {
            size_t** r = rmid;
            do {
                *r = mid;
                r++;
            } while (r != res_end);
        }
    }
}

void lower_bound_burst2(size_t* x, size_t** res)
{
    std::sort(x, x + K);

    #if 0
    struct Search
    {
        size_t* haystack_begin;
        size_t* haystack_end;
        size_t* needle_begin;
        size_t* needle_end;
        size_t** res_begin;
        size_t** res_end;
    };
    #endif

    burst2_internal(v, v + N, x, x + K, res, res + K);
}

void fnd_opt3()
{
    CTimer t;
    t.Start();
    std::ifstream f("data.txt");
    size_t r = 0;
    size_t x[16];
    size_t* res[16];
    for (size_t i = 0; i < M; i += K)
    {
        for (size_t k = 0; k < K; k++)
            f >> x[k];
        lower_bound_burst2(x, res);
        for (size_t k = 0; k < K; k++)
            r ^= *res[k];
    }
    t.Stop();
    COUTF(__func__, r, t.Mrps(M));
}


int main()
{
    wr();
    gen();
    rd();
    fnd();
    fnd_opt1();
    fnd_opt2();
    fnd_opt3();
    remove("data.txt");
}
