#include <iostream>

const unsigned long long MOD = 1000000000 + 9;

unsigned long long stupid_fibo(unsigned long long n)
{
	if(n <= 1)
		return 1;
	return (stupid_fibo(n - 1) + stupid_fibo(n - 2)) % MOD;
}

struct CMatrix2 {
	unsigned long long a[2][2];
	const CMatrix2 operator*(const CMatrix2& another) {
		CMatrix2 res;
		for(int i = 0;i < 2; i++)
			for(int j = 0; j < 2; j++)
				res.a[i][j] = (a[i][0] * another.a[0][j] + a[i][1] * another.a[1][j]) % MOD;
		return res;
	}
};

unsigned long long normal_fibo(unsigned long long n)
{
	if(n <= 1)
		return 1;
	CMatrix2 unit;
	unit.a[0][0] = unit.a[1][0] = unit.a[0][1] = 1;
	unit.a[1][1] = 0;
	CMatrix2 test;
	test.a[0][0] = test.a[1][1] = 1;
	test.a[0][1] = test.a[1][0] = 0;
	CMatrix2 pow = unit;
	while(n) {
		if(n & 1)
			test = test * pow;
		pow = pow * pow;
		n /= 2;
	}
	return test.a[0][0];
}


int main(int, char**)
{
	for(int i = 0; i < 40; i++)
		std::cout << i << " " << stupid_fibo(i) << " " << normal_fibo(i) << std::endl;
}