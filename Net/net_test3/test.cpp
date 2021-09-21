#include <iostream>
#include <alya.h>
#include <poll.h>
#include <signal.h>

using namespace alya;
using namespace alya::ipv4;

const unsigned short PORT = 55555;
const size_t BUF_SIZE = 1024;


int main()
{
	signal(SIGPIPE, SIG_IGN);

	try {
#ifdef SERVER
		CFDHolder list(Listen(PORT, true));
		COUTS("Listening", PORT, "...");
		CFDHolder s(Accept(list));
		COUTS("Accepted");
#endif
#ifdef CLIENT
		CFDHolder s(Connect("", PORT));
		COUTS("Connected");
#endif

		struct pollfd fds[2];
		fds[0].fd = 0;
		fds[1].fd = s;
		fds[0].events = fds[1].events = POLLIN;
		char buf[BUF_SIZE];
		COUTS("Waiting for commands: wr, sdr, sdw, sdrw");

		bool cont = true;
		while (cont) {
			fds[0].revents = fds[1].revents = 0;
			int r = poll(fds, 2, -1);
			if (r < 0)
				throw CNetException("poll failed");
			if (r == 0)
				continue;

			if (fds[0].revents & POLLIN) {
				size_t sz = BUF_SIZE - 1;
				if (!Read(0, buf, &sz)) {
					COUTS("Input closed");
					exit(EXIT_SUCCESS);
				}
				assert(sz > 0);
				sz--;
				buf[sz] = 0;
				if (strcmp(buf, "wr") == 0) {
					std::cout << "Writing... ";
					char c = '!';
					Write(s, &c, 1);
					COUTS("Wrote 1 byte");
				} else if (strcmp(buf, "sdr") == 0) {
					if (shutdown(s, SHUT_RD) < 0)
						throw CNetException("shutdown failed");
					COUTS("Shut down (R)");
					fds[1].events = 0;
					COUTS("Stopped reading event");
				} else if (strcmp(buf, "sdw") == 0) {
					if (shutdown(s, SHUT_WR) < 0)
						throw CNetException("shutdown failed");
					COUTS("Shut down (W)");
				} else if (strcmp(buf, "sdrw") == 0) {
					if (shutdown(s, SHUT_RDWR) < 0)
						throw CNetException("shutdown failed");
					COUTS("Shut down (WR)");
					fds[1].events = 0;
					COUTS("Stopped reading event");
				} else if (buf[0] != 0) {
					COUTS("Unknown command", buf);
				}
			}

			if (fds[1].revents & POLLERR) {
				COUTS("Peer err");
				cont = false;
			}
			if (fds[1].revents & POLLHUP) {
				COUTS("Peer hup");
				cont = false;
			}
			if (fds[1].revents & POLLNVAL) {
				COUTS("Peer nval");
				cont = false;
			}

			if (fds[1].revents & POLLIN) {
				std::cout << "Reading... ";
				size_t sz = BUF_SIZE - 1;
				if (!Read(s, buf, &sz)) {
					COUTS("Peer closed");
					fds[1].events = 0;
					COUTS("Stopped reading event");

					std::cout << "Writing bye... ";
					char c = '!';
					Write(s, &c, 1);
					COUTS("Wrote 1 byte");
				}
				if (sz > 0)
					COUTS("Peer sent", sz, "bytes");
			}
		}
	} catch (CNetException& e) {
		std::cerr << e.What() << std::endl;
	}
}
