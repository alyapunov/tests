#include <iostream>
#include <stdio.h>
#include <alya.h>
#include <stdlib.h>
#include <omp.h>

using namespace std;


const int Rounds = 25;
const size_t Size = 1024*1024*128;

unsigned long long a[Size], b[Size];

void f()
{
	cout << "F";
	for (size_t i = 0; i < Size; i++)
		b[i] = a[i];
}

void g()
{
	printf("G");
#pragma omp for
	//for (int i = 0; i < 10; i++)
	//	printf("!");
	for (size_t k = 0; k < Size; k++) {
		b[k] = a[k];
	}
}

void h()
{
	cout << "H";
#pragma omp parallel
	{
	//COUTF(omp_get_num_threads(), omp_get_thread_num());
	size_t blockSize = Size / omp_get_num_threads();
	size_t i1 = omp_get_thread_num() * blockSize;
	size_t i2 = (omp_get_thread_num() + 1) * blockSize;
	for (size_t i = i1; i < i2; i++)
		b[i] = a[i];
	}
}


void run()
{
	cout << "run" << endl;
	for (int i = 0; i < Rounds; i++) {
		g();
	}
}

int main(int, const char**)
{
	for (size_t i = 0; i < Size; i++)
		a[i] = i;
#pragma omp parallel
	{
//#pragma omp single
	run();
	}
	cout << endl;
	for (size_t i = 0; i < Size; i++)
		if (b[i] != i) {
			cout << "DEAFBEEF" << endl;
			break;
		}
	cout << endl;
}