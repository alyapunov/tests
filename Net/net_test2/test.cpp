/**
 * Client sends tons of small messages in non-blocking mode.
 * Server receives them with a big buffer.
 *
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

constexpr unsigned short PORT = 45000;
const char *addr = "127.0.0.1";
size_t client_msg_size;
bool run_server = false;
bool run_client = false;
constexpr size_t SRV_BUF_SIZE = 64 * 1024;
constexpr size_t NUM_MSG = 1 * 1024 * 1024;

size_t num_bytes_sent = 0;
size_t num_msg_sent_ok = 0;
size_t num_msg_sent_failed = 0;
size_t num_msg_sent_partial = 0;
double client_wall_time;
double client_cpu_time;

size_t num_reads = 0;
size_t num_bytes_read = 0;
double server_wall_time;
double server_cpu_time;

void
check(bool expr, const char *err_msg)
{
	if (!expr) {
		std::cerr << err_msg << std::endl;
		abort();
	}
}

double now_wall()
{
	timespec ts;
	int rc = clock_gettime(CLOCK_MONOTONIC, &ts);
	check(rc == 0, "clock_gettime(CLOCK_MONOTONIC) failed");
	return ts.tv_sec + ts.tv_nsec * 1e-9;
}

double now_cpu()
{
	timespec ts;
	int rc = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
	check(rc == 0, "clock_gettime(CLOCK_THREAD_CPUTIME_ID) failed");
	return ts.tv_sec + ts.tv_nsec * 1e-9;
}

void client_thread_f()
{
	int s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	check(s >= 0, "client socket failed");

	sockaddr_in saddr = {};
	saddr.sin_family = AF_INET;
	int rc = inet_pton(AF_INET, addr, &saddr.sin_addr);
	check(rc == 1, "inet_pton failed");
	saddr.sin_port = htons(PORT);
	rc = connect(s, (const sockaddr *) &saddr, sizeof(saddr));
	check(rc == 0, "connect failed");

	std::vector<char> buf(client_msg_size);

	client_wall_time = -now_wall();
	client_cpu_time = -now_cpu();
	for (size_t i = 0; i < NUM_MSG; i++) {
		rc = send(s, buf.data(), client_msg_size, MSG_DONTWAIT);
		if (rc > 0)
			num_bytes_sent += rc;
		if (rc < 0)
			num_msg_sent_failed++;
		else if ((size_t)rc == client_msg_size)
			num_msg_sent_ok++;
		else
			num_msg_sent_partial++;
	}
	client_wall_time += now_wall();
	client_cpu_time += now_cpu();

	rc = close(s);
	check(rc == 0, "client close failed");
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

	int s = accept(l, 0, 0);
	check(s >= 0, "accept failed");

	char buf[SRV_BUF_SIZE];

	server_wall_time = -now_wall();
	server_cpu_time = -now_cpu();
	while (true) {
		rc = read(s, buf, SRV_BUF_SIZE);
		check(rc >= 0, "read failed");
		if (rc == 0)
			break;
		num_reads++;
		num_bytes_read += rc;
	}
	server_wall_time += now_wall();
	server_cpu_time += now_cpu();

	rc = close(s);
	check(rc == 0, "server close (1) failed");
	rc = close(l);
	check(rc == 0, "server close (2) failed");
}

int usage(int argn, const char** args)
{
	const char *binary = argn ? args[0] : "binary";
	std::cout << "Usage: " << binary
		  << " server|client|both <msg size> [ip addr]" << std::endl;
	return 0;
}

int main(int argn, const char** args)
{
	if (argn < 3 || argn > 4)
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
	client_msg_size = atoi(args[2]);
	if (argn == 4)
		addr = args[3];

	std::vector<std::thread> threads;
	if (run_server)
		threads.emplace_back(server_thread_f);

	if (run_client)
		threads.emplace_back(client_thread_f);

	for (auto& thread : threads)
		thread.join();

	if (run_client) {
		double Mrps_wall = 1e-6 * NUM_MSG / client_wall_time;
		double Mrps_cpu = 1e-6 * NUM_MSG / client_cpu_time;
		std::cout << "Client stats:"
			  << "\nnum_bytes_sent = " << num_bytes_sent
			  << "\nnum_msg_sent_ok = " << num_msg_sent_ok
			  << "\nnum_msg_sent_failed = " << num_msg_sent_failed
			  << "\nnum_msg_sent_partial = " << num_msg_sent_partial
			  << "\nMrps_wall = " << Mrps_wall
			  << "\nMrps_cpu = " << Mrps_cpu
			  << "\n";
	}

	if (run_server) {
		double avg_read_size = double(num_bytes_read) / num_reads;
		double Mrps_wall = 1e-6 * num_reads / server_wall_time;
		double Mrps_cpu = 1e-6 * num_reads / server_cpu_time;
		std::cout << "Server stats:"
			  << "\nnum_bytes_read = " << num_bytes_read
			  << "\navg_read_size = " << avg_read_size
			  << "\nnum_reads = " << num_reads
			  << "\nMrps_wall = " << Mrps_wall
			  << "\nMrps_cpu = " << Mrps_cpu
			  << "\n";
	}
}
