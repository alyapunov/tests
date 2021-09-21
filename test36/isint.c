#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

typedef struct {
    const char* in;
    bool out;
} test_case_t;

static const test_case_t tests[] = {
    {"", false},
    {"1", true},
    {"-2", true},
    {"1030", true},
    {"-567", true},
    {"28a5", false},
    {"32.8", false},
    {"1e+99", false},
    {"2147483647", true},   // Hardcoded value for 4-byte int
    {"2147483648", false},   // Hardcoded value for 4-byte int
    {"-2147483648", true},  // Hardcoded value for 4-byte int
    {"-2147483649", false},  // Hardcoded value for 4-byte int
    {"Hello there!", false},
    {"100000000000", false},
    {"-100000000000", false},
};

static size_t
my_strlen(const char* str)
{
    size_t len = 0;
    while(str[len]) {++len;}
    return len;
}

#define atoi_char(c) ((int)(c - '0'))
#define is_digit(c) ((c >= '0') && (c <= '9'))

bool
is_int(const char *number)
{
    const size_t len = my_strlen(number);

    if(len == 0) {
        return false;
    }

    int num = 0;
    const size_t sign = (number[0] == '-');

    for(size_t i = sign; i < my_strlen(number); ++i) {
        const char c = number[i];

        if(is_digit(c)) {
            const int digit = atoi_char(c);

            if(num < INT_MAX / 10) {
                num = num * 10 + digit;
            } else if((num == INT_MAX / 10) && (i == len - 1)) {
                return ((INT_MAX - num * 10 + (int)sign) >= digit);
            } else {
                return false;
            }

        } else {
            return false;
        }
    }

    return true;
}

int
main()
{
    printf("INT_MAX: %d\r\n", INT_MAX);
    printf("INT_MIN: %d\r\n\n", INT_MIN);

    const size_t test_count = sizeof(tests)/sizeof(test_case_t);

    for(size_t i = 0; i < test_count; ++i) {
        const bool result = (is_int(tests[i].in) == tests[i].out);
        printf("TEST #%02lu: %s\r\n", i, result ? "PASS" : "FAIL");
    }

    return 0;
}
