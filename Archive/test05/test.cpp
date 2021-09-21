#include <alya.h>
#include <stdio.h>

int main()
{
	volatile double d1 = 1.;
	volatile double d2 = 3.;
	volatile double d3 = 1. / 3.;


	//printf("d1 = %f, d2 = %f, d3 = %f\n", d1, d2, d3);
	printf("d1 = %lf, d2 = %lf, d3 = %lf\n", d1, d2, d3);
	printf("%d %d %d\n", int(d1 * 100), int(d2 * 100), int(d3 * 100));
	//COUTF(d1, d2, d3);
	COUTF((d3 == d1 / d2)); 
}
