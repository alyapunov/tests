#include <vector>
#include <iostream>

using pos_t = unsigned int;
//using pos_t = int;
//using pos_t = unsigned long; // <-- here

using val_t = unsigned int;

const pos_t SIZE = 2;

struct my_pair
{
    val_t a, b;
    my_pair(val_t x = 0, val_t y = 0) : a(x), b(y) {} // <-- here
};

static my_pair sdata[SIZE];

struct cont
{
    pos_t pos = 0;

    my_pair data[SIZE];
    //my_pair* data = sdata; // <-- here
    //std::vector<my_pair> data = std::vector<my_pair>{SIZE}; // <-- or here
};

int main() {
    cont c;
    for (val_t i = 0; i < 1000000000; i++)
    {
        my_pair& v = c.data[++c.pos % SIZE];
        v = my_pair{i, i};
        //v.a = i; // <-- here
        v.b = i; // <-- here
    }
    std::cout << c.data[0].a << std::endl;
    return c.data[0].a;
}
