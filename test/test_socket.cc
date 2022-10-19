#include "src/socket.h"
#include "src/alotz.h"
#include "src/iomanager.h"

static alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

void test_socket() {
    alotz::IPAddress::ptr addr = alotz::Address::LookupAnyIPAddress("www.baidu.com");
    if (addr) {
        ALOTZ_LOG_INFO(g_logger) << "get address: " << addr->toString();
    } else {
        ALOTZ_LOG_ERROR(g_logger) << "get address fail";
        return;
    }

    alotz::Socket::ptr sock = alotz::Socket::CreateTCP(addr);
    addr->setPort(80);
    ALOTZ_LOG_INFO(g_logger) << "addr=" << addr->toString();
    if (!sock->connect(addr)) {
        ALOTZ_LOG_INFO(g_logger) << "connect " << addr->toString() << " fail";
        return;
    } else {
        ALOTZ_LOG_INFO(g_logger) << "connect " << addr->toString() << " connected";
    }

    const char buff[] = "GET / HTTP/1.0\r\n\r\n";
    int rt = sock->send(buff, sizeof(buff));
    if (rt <= 0) {
        ALOTZ_LOG_INFO(g_logger) << "send fail rt=" << rt;
        return;
    }

    std::string buffs;
    buffs.resize(4096);
    rt = sock->recv(&buffs[0], buffs.size());

    if (rt <= 0) {
        ALOTZ_LOG_INFO(g_logger) << "recv fail rt=" << rt;
        return;
    }

    buffs.resize(rt);
    ALOTZ_LOG_INFO(g_logger) << buffs;
}

int main(int argc, char** argv) {
    alotz::IOManager iom;
    iom.schedule(&test_socket);
    return 0;
}