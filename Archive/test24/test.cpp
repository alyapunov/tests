#include <sys/socket.h> // socket() ?
#include <netinet/in.h> // htons, sockaddr_in, constants
#include <arpa/inet.h> // aton
#include <unistd.h> // close
#include <fcntl.h> // fcntl
#include <errno.h> // errno


#include <alya.h>
#include <string.h>
#include <thread>
#include <atomic>


using namespace alya::ipv4;

const char* req =
"GET /get?protocol=http&debug=0&pad_id=6049&vk_id=19836&ibanner_id=43585887&template_id=74&tstr_set3=Android HTTP/1.1\r\n"
"Host: rbbanner95.trgd.mail.ru\r\n"
"Cookie: Mpop=1565603530:6f6c670e445958071905000017031f051c054f6c5150445e05190401041d50175c4e50494c565e4610545a45451f5956505d1e444d:a.lyapunov@corp.mail.ru:\r\n"
"\r\n";

const size_t num_threads = 10;

const size_t ibuf_size = 256 * 1024;
thread_local char ibuf[ibuf_size];

std::atomic<bool> stop{false};

class CNetException {
public:
    CNetException(const std::string &_message) : message(_message) {}
    const std::string& what() const { return message; }
    operator const std::string&() const { return message; }
    friend std::ostream& operator << (std::ostream& strm, const CNetException& e) { strm << e.what(); return strm; }
private:
    std::string message;
};

class CFDHolder {
public:
    CFDHolder(int fd_) : fd(fd_) {}
    ~CFDHolder() { if(fd >= 0) close(fd); }
    void Reset(int fd_ = -1) { fd = fd_; }
    operator int() { return fd; }
private:
    int fd;
};

sockaddr_in
parseAddr(const char *ip, unsigned short port)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    if (ip) {
        int r;
        if (ip[0])
            r = inet_pton(AF_INET, ip, &addr.sin_addr);
        else
            //addr.sin_addr.s_addr = 0x1000007F, r = 1;
            r = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
        if (r == 0)
            throw CNetException("inet_pton() failed");
    } else {
        addr.sin_addr.s_addr = INADDR_ANY;
    }
    addr.sin_port = htons(port);
    return addr;
}

int
connectOrDie(const char* ip, short port)
{
    sockaddr_in addr = parseAddr(ip ? ip : "", port);
    int s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0)
        throw CNetException("socket() failed");
    int r = connect(s, (const sockaddr *)&addr, sizeof(addr));
    if(r)
    {
        close(s);
        throw CNetException("connect() failed");
    }
    return s;
}

void writeOrDie(int s, const char* data, size_t size)
{
    const char* p = data;
    size_t left = size;
    while (left > 0)
    {
        ssize_t n = write(s, p, left);
        if (n < 0) {
            if (errno == EWOULDBLOCK)
                errno = EINTR;
            switch (errno) {
            case EAGAIN:
            case EINTR:
                continue;
            default:
                throw CNetException("write failed");
            }
        }
        p += n;
        left -= n;
    }
}

size_t readOrDie(int s, char* data, size_t limit)
{
    while (true)
    {
        ssize_t n = read(s, data, limit);
        if (n < 0)
        {
            if (errno == EWOULDBLOCK)
                errno = EINTR;
            switch (errno) {
            case EAGAIN:
            case EINTR:
                continue;
            default:
                throw CNetException("read failed");
            }
        }
        else if (n == 0)
        {
            throw CNetException("socket was closed by peer");
        }
        return n;
    }

}

void thread_f()
{
    size_t total_requests = 0;
    size_t total_bytes = 0;
    time_t start_time = time(nullptr);
    while (!stop)
    {
        try
        {
            CFDHolder s(connectOrDie("128.140.171.150", 80));
            while (!stop)
            {
                writeOrDie(s, req, strlen(req));

                size_t header_size = 0;
                char* rbuf = ibuf;
                size_t left = ibuf_size - 1;
                while (true)
                {
                    size_t r = readOrDie(s, rbuf, left);
                    rbuf += r;
                    left -= r;
                    *rbuf = 0;
                    char *found = strstr(ibuf, "\r\n\r\n");
                    if (nullptr != found)
                    {
                        header_size = (found + 4) - ibuf;
                        *found = 0;
                        break;
                    }
                }

                const char* expect = "HTTP/1.1 200 OK\r\n";
                if (header_size < strlen(expect) || 0 != memcmp(ibuf, expect, strlen(expect)))
                    throw CNetException("HTTP not OK");

                const char* cl = "Content-Length: ";
                char *found = strstr(ibuf, cl);
                if (nullptr == found)
                    throw CNetException("no Content-Length");

                size_t content_length = atoi(found + strlen(cl));

                size_t content_read = (rbuf - ibuf) - header_size;
                while (content_read < content_length)
                {
                    size_t r = readOrDie(s, ibuf, std::min(ibuf_size, content_length - content_read));
                    content_read += r;
                }

                total_requests++;
                total_bytes += content_length;
            }
        }
        catch (const CNetException& e)
        {
            COUTF(e.what());
        }
    }
    time_t end_time = time(nullptr);
    std::cout << "R:" << total_requests << " B:" <<  total_bytes << " T:" << (end_time - start_time) << std::endl;
}

int main(int, const char**)
{
    std::thread t[num_threads];
    for (size_t i = 0; i < num_threads; i++)
        t[i] = std::thread(thread_f);
    sleep(120);
    stop = true;
    for (size_t i = 0; i < num_threads; i++)
        t[i].join();
}
