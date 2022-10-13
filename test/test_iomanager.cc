#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>
#include "src/alotz.h"

alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

int sock = 0;

void test_fiber() {
    ALOTZ_LOG_INFO(g_logger) << "test_fiber sock=" << sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "115.239.210.27", &addr.sin_addr.s_addr);

    if (!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
    } else if (errno == EINPROGRESS) {
        ALOTZ_LOG_INFO(g_logger) << "add event errno= " << errno << " " << strerror(errno);
        alotz::IOManager::GetThis()->addEvent(sock, alotz::IOManager::READ, []() {
            ALOTZ_LOG_INFO(g_logger) << "read callback";
        });
        alotz::IOManager::GetThis()->addEvent(sock, alotz::IOManager::WRITE, []() {
            ALOTZ_LOG_INFO(g_logger) << "write callback";
            alotz::IOManager::GetThis()->cancelEvent(sock, alotz::IOManager::READ);
            close(sock);
        });
    } else {
        ALOTZ_LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
    }
}

void test1() {
    std::cout << "EPOLLIN=" << EPOLLIN
        << "EPOLLOUT=" << EPOLLOUT << std::endl;
    alotz::IOManager iom(2, false);
    iom.schedule(&test_fiber);
}

alotz::Timer::ptr s_timer;

void test_timer() {
    alotz::IOManager iom(2);
    s_timer = iom.addTimer(1000, []() {
        static int i = 0;
        ALOTZ_LOG_INFO(g_logger) << "hello timer i= " << i;
        if (++i == 3) {
            s_timer->reset(2000, true);
        }
    }, true);
}

int main(int argc, char** argv) {
    // test1();
    test_timer();
    return 0;
}