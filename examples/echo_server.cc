#include "src/tcp_server.h"
#include "src/log.h"
#include "src/iomanager.h"
#include "src/bytearray.h"
#include "src/address.h"

static alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

class EchoServer : public alotz::TcpServer {
public:
    EchoServer(int type);
    void handleClient(alotz::Socket::ptr client);

private:
    int m_type = 0;
};

EchoServer::EchoServer(int type)
    : m_type(type) {
}

void EchoServer::handleClient(alotz::Socket::ptr client) {
    ALOTZ_LOG_INFO(g_logger) << "handleClient " << *client;
    alotz::ByteArray::ptr ba(new alotz::ByteArray);
    while (true) {
        ba->clear();
        std::vector<iovec> iovs;
        ba->getWriteBuffers(iovs, 1024);

        int rt = client->recv(&iovs[0], iovs.size());
        if (rt == 0) {
            ALOTZ_LOG_INFO(g_logger) << "client close: " << * client;
            break;
        } else if (rt < 0) {
            ALOTZ_LOG_INFO(g_logger) << "client error rt= " << rt
                << " errno=" << errno << " errstr=" << strerror(errno);
            break;
        }
        ba->setPosition(ba->getPosition() + rt);
        ba->setPosition(0);
        if (m_type == 1) {
            std::cout << ba->toString();
        } else {
            std::cout << ba->toHexString();
        }
        std::cout.flush();
    }
}

int type = 1;

void run() {
    ALOTZ_LOG_INFO(g_logger) << "server type= " << type;
    EchoServer::ptr es(new EchoServer(type));
    auto addr = alotz::Address::LookupAny("0.0.0.0:8020");
    while (es->bind(addr)) {
        sleep(2);
    }
    es->start();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        ALOTZ_LOG_INFO(g_logger) << "used as[" << argv[0] << "-t] or [" << argv[0] << " -b]";
        return 0;
    }
    if (strcmp(argv[1], "-b")) {
        type = 2;
    }

    alotz::IOManager iom(2);
    iom.schedule(run);
    return 0;
}