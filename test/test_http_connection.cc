#include <iostream>
#include "src/http/http_connection.h"
#include "src/log.h"
#include "src/iomanager.h"
#include <fstream>

static alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

void run() {
    alotz::Address::ptr addr = alotz::Address::LookupAnyIPAddress("www.baidu.com:80");
    if (!addr) {
        ALOTZ_LOG_INFO(g_logger) << "get addr error";
        return;
    }

    alotz::Socket::ptr sock = alotz::Socket::CreateTCP(addr);
    bool rt = sock->connect(addr);
    if (!rt) {
        ALOTZ_LOG_INFO(g_logger) << "connect " << *addr << " failed";
        return;
    }

    alotz::http::HttpConnection::ptr conn(new alotz::http::HttpConnection(sock));
    alotz::http::HttpRequest::ptr req(new alotz::http::HttpRequest);
    req->setPath("/blog");
    req->setHeader("host", "www.sylar.top");
    ALOTZ_LOG_INFO(g_logger) << "req: " << std::endl
        << *req;
    conn->sendRequest(req);
    auto rsp = conn->recvResponse();

    if (!rsp) {
        ALOTZ_LOG_INFO(g_logger) << "recv response error";
        return;
    }
    ALOTZ_LOG_INFO(g_logger) << "rsp:" << std::endl
        << *rsp;
    
    std::ofstream ofs("rsp.dat");
    ofs << *rsp;
}

int main(int argc, char** argv) {
    alotz::IOManager iom(2);
    iom.schedule(run);
    return 0;
}