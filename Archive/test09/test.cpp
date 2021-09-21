#include <iostream>
#include <alya.h>
#include <omp.h>

using namespace std;


double poor_cache_opt(int ** matrix, long int size){

    double total_begin = omp_get_wtime();

    for(int x = 0; x < size; x++){
        for(int y = 0; y < size; y++){
            matrix[y][x] = matrix[y][x] * 2;
        }
    }
    double total_end = omp_get_wtime();
    return double(total_end - total_begin);
}

double proper_cache_opt(int ** matrix, long int size){

    double total_begin = omp_get_wtime();

    for(int x = 0; x < size; x++){
        for(int y = 0; y < size; y++){
            matrix[x][y] = matrix[x][y] * 2;
        }
    }

    double total_end = omp_get_wtime();
    return double(total_end - total_begin);
}

double cache_opt(int ** matrix, long int size, int blocksize){

    double total_begin = omp_get_wtime();

    for(int x = 0; x < size; x += blocksize){
        for(int i = x; i < x + blocksize; i++){
            for(int y = 0; y < size; y+= blocksize){
                __builtin_prefetch(&matrix[i][y + blocksize]);
                for(int j = y; j < y + blocksize; j++){
                    matrix[i][j] = matrix[i][j] * 2;
                }
            }
        }
    }

    double total_end = omp_get_wtime();
    return double(total_end - total_begin);
}

double thrash_openmp(int ** matrix, long int size){

    double total_begin = omp_get_wtime();

    #pragma omp parallel for
    for(int x = 0; x < size; x++){
        for(int y = 0; y < size; y++){
            matrix[y][x] = matrix[y][x] * 2;
        }
    }
    double total_end = omp_get_wtime();
    return double(total_end - total_begin);
}

double possible_thrash_openmp(int ** matrix, long int size){

  double total_begin = omp_get_wtime();

  #pragma omp parallel for schedule(dynamic)
  for(int x = 0; x < size; x++){
    for(int y = 0; y < size; y++){
      matrix[x][y] = matrix[x][y] * 2;
    }
  }
  double total_end = omp_get_wtime();
  return double(total_end - total_begin);
}

double bad_openmp_cache_opt(int ** matrix, long int size){

    double total_begin = omp_get_wtime();

    for(int x = 0; x < size; x++){
        if(x + 1 < size){ __builtin_prefetch(matrix[x+1]); }
        #pragma omp parallel for
        for(int y = 0; y < size; y++){
            matrix[x][y] = matrix[x][y] * 2;
        }
    }

    double total_end = omp_get_wtime();
    return double(total_end - total_begin);
}

double good_openmp_cache_opt(int ** matrix, long int size){

    double total_begin = omp_get_wtime();

    #pragma omp parallel for schedule(static)
    for(int x = 0; x < size; x++){
        for(int y = 0; y < size; y++){
            matrix[x][y] = matrix[x][y] * 2;
        }
    }

    double total_end = omp_get_wtime();
    return double(total_end - total_begin);

}

void reset(int ** p, long int size)
{
	for (int i = 0; i < size; i++)
		for(int j = 0; j < size; j++)
			p[i][j] = i * size + j;
}

int main(int, const char**)
{
	const int size = 1024*32;
	int **p = new int *[size];
	for (int i = 0; i < size; i++)
		p[i] = new int[size];

	reset(p, size);
	COUTF(poor_cache_opt(p, size));
	reset(p, size);
	COUTF(proper_cache_opt(p, size));
	for(int k = 4; k <= 1024; k *= 2)
		COUTF(cache_opt(p, size, k), k);
	reset(p, size);
	COUTF(thrash_openmp(p, size));
	reset(p, size);
	COUTF(possible_thrash_openmp(p, size));
	reset(p, size);
	COUTF(bad_openmp_cache_opt(p, size));
	reset(p, size);
	COUTF(good_openmp_cache_opt(p, size));

	for (int i = 0; i < size; i++)
		delete [] p[i];
	delete [] p;
}
