/*
 * Clients connect, send a short message and close.
 * Server counts them all.

 * Build with:
 * g++ -std=c++11 -pthread -DNDEBUG -O2 test.cpp
 */

#include <stddef.h>
#include <sys/socket.h>
#include <arpa/inet.h> // pton
#include <netinet/in.h> // htons, sockaddr_in, constants
#include <unistd.h>
#include <poll.h>

#include <cassert>
#include <iostream>
#include <thread>
#include <vector>
#include <string>

constexpr size_t NUM_CLIENTS = 20;
constexpr size_t MSG_PER_CLIENT = 20;
bool run_server = false;
bool run_client = false;
const char *addr = "127.0.0.1";
constexpr unsigned short PORT = 45000;

void check(bool expr, const char *err_msg)
{
	if (!expr) {
		std::cerr << err_msg << std::endl;
		abort();
	}
}

void client_thread_f()
{
	for (size_t i = 0; i < MSG_PER_CLIENT; i++) {
		int s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		check(s >= 0, "client socket failed");

		sockaddr_in saddr = {};
		saddr.sin_family = AF_INET;
		int rc = inet_pton(AF_INET, addr, &saddr.sin_addr);
		check(rc == 1, "inet_pton failed");
		saddr.sin_port = htons(PORT);

		rc = connect(s, (const sockaddr *) &saddr, sizeof(saddr));
		check(rc == 0, "connect failed");

		rc = write(s, "Hello mf", 9);
		check(rc == 9, "write failed");

		rc = close(s);
		check(rc == 0, "client close failed");
	}
}

void server_thread_f()
{
	int l = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	check(l >= 0, "listen socket failed");

	sockaddr_in saddr = {};
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = INADDR_ANY;
	saddr.sin_port = htons(PORT);

	int rc = bind(l, (const sockaddr *)&saddr, sizeof(saddr));
	check(rc == 0, "bind failed");

	rc = listen(l, SOMAXCONN);
	check(rc == 0, "listen failed");

	std::cout << "Num of clients expected: " << NUM_CLIENTS * MSG_PER_CLIENT << std::endl;
	size_t num_conn = 0;
	size_t num_data = 0;
	size_t num_fin = 0;

	std::vector<pollfd> polls(1);
	polls[0].fd = l;
	polls[0].events = POLLIN;
	while (num_fin != NUM_CLIENTS * MSG_PER_CLIENT) {
		int polled = poll(polls.data(), polls.size(), -1);
		check(polled > 0, "poll failed");
		check((polls[0].revents & ~POLLIN) == 0, "poll listen error");
		if (polls[0].revents & POLLIN) {
			int s = accept(l, 0, 0);
			check(s >= 0, "accept failed");
			polls.emplace_back();
			polls.back().fd = s;
			polls.back().events = POLLIN;
			polls.back().revents = 0;
			num_conn++;
		}
		for (size_t i = 1; i < polls.size(); i++) {
			check((polls[i].revents & ~POLLIN) == 0, "poll conn error");
			if ((polls[i].revents & POLLIN) == 0)
				continue;
			char buf[16];
			int rd = read(polls[i].fd, buf, sizeof(buf));
			check(rd >= 0, "read failed");

			if (rd > 0) {
				num_data++;
				check(rd == 9, "i dont care");
			} else {
				num_fin++;
				rc = close(polls[i].fd);
				check(rc == 0, "server close failed");
				polls[i] = polls.back();
				polls.pop_back();
				i--;
			}
		}
	}

	std::cout << "Num of clients that connected: " << num_conn << std::endl;
	std::cout << "Num of clients that sent data: " << num_data << std::endl;
	std::cout << "Num of clients closed peer: " << num_fin << std::endl;

	rc = close(l);
	check(rc == 0, "listen close failed");

}

int usage(int argn, const char** args)
{
	const char *binary = argn ? args[0] : "binary";
	std::cout << "Usage: " << binary
		  << " server|client|both [ip addr]" << std::endl;
	return 0;
}

int main(int argn, const char** args)
{
	if (argn < 2 || argn > 3)
		return usage(argn, args);
	std::string variant = args[1];
	if (variant == "server")
		run_server = true;
	else if (variant == "client")
		run_client = true;
	else if (variant == "both")
		run_server = run_client = true;
	else
		return usage(argn, args);
	if (argn == 4)
		addr = args[3];

	std::vector<std::thread> threads;

	if (run_client) {
		threads.emplace_back(server_thread_f);
	}

	if (run_client) {
		for (size_t i = 0; i < NUM_CLIENTS; i++)
			threads.emplace_back(client_thread_f);
	}

	for (auto& thread : threads)
		thread.join();
}