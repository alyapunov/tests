#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#include <alya.h>

const char* filename = "/home/alyapunov/Work/data.dat";
//const char* filename = "/data/data.dat";
const size_t LOG2_SIZE = 36;
const size_t SIZE = 1ull << LOG2_SIZE;
const size_t LOG2_COUNT = LOG2_SIZE - 3;
const size_t COUNT = SIZE / 8;
const size_t TEST_COUNT = 64 * 1024;

int fd;
void* m;
uint64_t* data;

void die(const char* message)
{
    COUTS(message, strerror(errno));
    abort();
}

void check(bool expr, const char* message)
{
    if (!expr)
        die(message);
}

void fill()
{
    uint64_t* data = (uint64_t*)m;
    alya::CTimer t;
    t.Start();
    uint64_t s = 0;
    for (size_t i = 0; i < COUNT; i++)
    {
        size_t n = i;
        for (size_t b = 1ull << (LOG2_COUNT - 1); b; b >>= 1)
        {
            if (n & b)
            {
                n ^= b;
            }
            else
            {
                n |= b;
                break;
            }
        }
        data[i] = n;
        s += n;
        if (i % 512 == 0)
            msync(m, SIZE, MS_ASYNC);
    }
    t.Stop();
    COUTF("sequent fill", t.krps(COUNT));
    COUTF(s);
}

size_t pos = 0;

void read_test()
{
    uint64_t* data = (uint64_t*)m;
    alya::CTimer t;
    t.Start();
    uint64_t s = 0;
    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        uint64_t& v = data[pos];
        s += v;
        pos = v;
    }
    t.Stop();
    COUTF("rand read", t.krps(TEST_COUNT));
    COUTF(s);
}

void write_test()
{
    uint64_t* data = (uint64_t*)m;
    alya::CTimer t;
    t.Start();
    uint64_t s = 0;
    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        uint64_t& v = data[pos];
        s += v;
        pos = v;
        v = 0;
        msync((void*)((((intptr_t)&v) & ~4095ull)), 4096, MS_ASYNC);
    }
    t.Stop();
    COUTF("rand write", t.krps(TEST_COUNT));
    COUTF(s);
}

void info()
{
    size_t expected_s = 0;
    for (size_t i = 0; i < COUNT; i++)
        expected_s += i;
    COUTF(expected_s);
}

int main()
{
    fd = open(filename, O_TRUNC | O_CREAT | O_RDWR | O_LARGEFILE, 0777);
    check(fd >= 0, "open");
    int rc = ftruncate(fd, SIZE);
    check(rc == 0, "ftruncate");
    m = mmap(nullptr, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    check(m != MAP_FAILED, "mmap");

    info();
    fill();
    read_test();
    write_test();

    rc = munmap(m, SIZE);
    check(rc == 0, "munmap");
    close(fd);

    COUT("done");
}
