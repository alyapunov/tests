#include <alya.h>
#include <string>
#include <iostream>
#include <alya/alya.net.h>
#include <string.h>

using namespace std;
using namespace alya::ipv4;

static void
report(const char* func)
{
    cout << func << " failed: " << strerror(errno) << endl;
}

int main()
{
    string s;
    int port = 40123;
    int lsock = -1;
    int sock = -1;
    
    
    while (true)
    {
        cin >> s;
        if (s == "exit")
        {
            exit(0);
        }
        else if (s == "listen")
        {
            lsock = Socket();
            sockaddr_in addr = Addr(0, port);
            int r = bind(lsock, (const sockaddr *)&addr, sizeof(addr));
            if (r)
            {
                report("bind");
                close(lsock);
            }
            else
            {
                r = listen(lsock, SOMAXCONN);
                if (r)
                {
                    report("listen");
                    close(lsock);
                }
            }
        }
        else if (s == "accept")
        {
            sock = accept(lsock, 0, 0);
            if (sock < 0)
                report("accept");
            else
                cout << "accepted" << endl;
        }
        else if (s == "connect")
        {
            sock = Socket();
            sockaddr_in addr = Addr(0, port);
            int r = connect(sock, (const sockaddr *)&addr, sizeof(addr));
            if (r)
            {
                report("connect");
                close(sock);
            }
            else
                cout << "connected" << endl;
        }
        else if (s == "write")
        {
            char buf[16] = {0};
            int r = write(sock, buf, 16);
            cout << "write result: " << r << endl;
            if (r <= 0)
                report("write");
        }
        else if (s == "read")
        {
            char buf[16] = {0};
            int r = read(sock, buf, 16);
            cout << "read result: " << r << endl;
            if (r < 0)
                report("read");
        }
        else if (s == "close")
        {
            int r = close(sock);
            if (r)
                report("close");
        }
        else if (s == "shutdownr")
        {
            int r = shutdown(sock, SHUT_RD);
            if (r)
                report("shutdown");
        }
        else if (s == "shutdownw")
        {
            int r = shutdown(sock, SHUT_WR);
            if (r)
                report("shutdown");
        }
        else
        {
            cout << "listen/accept/connect/read/write/close/shutdownr/shutdownw/exit" << endl;
        }
    }
        
}

