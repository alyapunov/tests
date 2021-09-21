#include <iostream>
#include <alya.h>
#include <sys/poll.h>

using namespace std;
using namespace alya;
using namespace ipv4;

const size_t buf_size = 4096;

struct CBuf {
	char buf[buf_size];
	const char *begin;
	size_t size;
	bool OnRead(size_t _size) { begin = buf; size = _size; return size != 0; }
	bool OnWrite(size_t _size) { begin += _size; size -= _size; return size == 0; }
};


void run()
{
	int port_from = 44445;
	int port_to = 33013;
	int l = Listen(port_from, true);
	int a = Accept(l);
	COUT("accepted");
	int s = Connect("127.0.0.1", port_to);
	COUT("connected");
	CBuf bufs[2];
	struct pollfd polldata[2];
	polldata[0].fd = a;
	polldata[0].events = POLLIN;
	polldata[1].fd = s;
	polldata[1].events = POLLIN;
	SetNonBlock(a);
	SetNonBlock(s);
	bool done = false;
	while (!done) {
		int p = poll(polldata, 2, -1);
		if (p <= 0)
			throw CNetException("poll failed");
		for (int i = 0, j = 1; i < 2; i++, j--) {
			if (polldata[i].revents & POLLIN) {
				size_t size = buf_size;
				if (!Read(polldata[i].fd, bufs[i].buf, &size)) {
					COUT("Closed ", i == 0 ? "from" : "to");
					done = true;
					break;
				}
				COUTS("read:", size, i == 0 ? "from" : "to");
				cout << "\"";
				for (size_t k = 0; k < size; k++) {
					int code = (unsigned)(unsigned char)bufs[i].buf[k];
					cout << "\\" << code / 64 << code / 8 % 8 << code % 8;
				}
				cout << "\"\n";
				if (bufs[i].OnRead(size)) {
					polldata[i].events &= ~POLLIN;
					polldata[j].events |= POLLOUT;
				}
			} else if (polldata[i].revents & POLLOUT) {
				size_t size = bufs[j].size;
				Write(polldata[i].fd, bufs[j].begin, &size);
				COUTS("write:", size, i == 0 ? "from" : "to");
				if (bufs[j].OnWrite(size)) {
					polldata[i].events &= ~POLLOUT;
					polldata[j].events |= POLLIN;
				}
			}
		}
	}
	Close(s);
	Close(a);
	Close(l);
}

const char message[] = "\021\000\000\000\035\000\000\000\001\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\377\377\377\377\001\000\000\000\001\000\000\000\004\000\000\000\000";

void test() {
	int port_to = 33013;
	int s = Connect("127.0.0.1", port_to);
	const size_t size = sizeof(message) - 1;
	while (true) {
		Write(s, message, size);
	}
}

int main(int n, const char**)
{
	try {
		test();
		run();
	} catch (CNetException &e) {
		COUTF(e.What());
	}
}
