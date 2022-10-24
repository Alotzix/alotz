#include "src/tcp_server.h"
#include "src/iomanager.h"
#include "src/log.h"

alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

void run() {
    auto addr = alotz::Address::LookupAny("0.0.0.0:8033");

    std::vector<alotz::Address::ptr> addrs;
    addrs.push_back(addr);

    alotz::TcpServer::ptr tcp_server(new alotz::TcpServer);
    std::vector<alotz::Address::ptr> fails;
    while (!tcp_server->bind(addrs, fails)) {
        sleep(2);
    }
    tcp_server->start();
}

int main(int argc, char** argv) {
    alotz::IOManager iom(2);
    iom.schedule(run);
    return 0;
}