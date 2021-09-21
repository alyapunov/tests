#include <iostream>
#include <thread>
#include <chrono>
#include "fifo.h"

size_t N = 16 * 1024 * 1024;
size_t M = 1024;

struct lock_free_fifo* fifo;

void producer()
{
	for (size_t i = 0; i < N; i++) {
		void *msg = nullptr;
		while (lock_free_fifo_put(fifo, &msg, 1) == 0);
	}
}

void consumer()
{
	for (size_t i = 0; i < N; i++) {
		void *msg;
		while (lock_free_fifo_get(fifo, &msg, 1) == 0);
	}
}

int main()
{
	fifo = (struct lock_free_fifo*) malloc(sizeof(*fifo) + M * sizeof(void*));
	lock_free_fifo_init(fifo, M);

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	std::thread th1(consumer);
	std::thread th2(producer);


	th2.join();
	th1.join();

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	free(fifo);

	double duration = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();

	std::cout << (N / duration / 1e6) << " Mrps\n";
}
