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

void test2() {
    alotz::IPAddress::ptr addr = alotz::Address::LookupAnyIPAddress("www.baidu.com:80");
    if(addr) {
        ALOTZ_LOG_INFO(g_logger) << "get address: " << addr->toString();
    } else {
        ALOTZ_LOG_ERROR(g_logger) << "get address fail";
        return;
    }

    alotz::Socket::ptr sock = alotz::Socket::CreateTCP(addr);
    if(!sock->connect(addr)) {
        ALOTZ_LOG_ERROR(g_logger) << "connect " << addr->toString() << " fail";
        return;
    } else {
        ALOTZ_LOG_INFO(g_logger) << "connect " << addr->toString() << " connected";
    }

    uint64_t ts = alotz::GetCurrentUS();
    for(size_t i = 0; i < 10000000000ul; ++i) {
        if(int err = sock->getError()) {
            ALOTZ_LOG_INFO(g_logger) << "err=" << err << " errstr=" << strerror(err);
            break;
        }

        //struct tcp_info tcp_info;
        //if(!sock->getOption(IPPROTO_TCP, TCP_INFO, tcp_info)) {
        //    SYLAR_LOG_INFO(g_looger) << "err";
        //    break;
        //}
        //if(tcp_info.tcpi_state != TCP_ESTABLISHED) {
        //    SYLAR_LOG_INFO(g_looger)
        //            << " state=" << (int)tcp_info.tcpi_state;
        //    break;
        //}
        static int batch = 10000000;
        if(i && (i % batch) == 0) {
            uint64_t ts2 = alotz::GetCurrentUS();
            ALOTZ_LOG_INFO(g_logger) << "i=" << i << " used: " << ((ts2 - ts) * 1.0 / batch) << " us";
            ts = ts2;
        }
    }
}

int main(int argc, char** argv) {
    alotz::IOManager iom;
    iom.schedule(&test2);
    return 0;
}