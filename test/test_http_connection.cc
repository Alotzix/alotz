#include <iostream>
#include "src/http/http_connection.h"
#include "src/log.h"
#include "src/iomanager.h"
#include <fstream>

static alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

void test_pool() {
    alotz::http::HttpConnectionPool::ptr pool(new alotz::http::HttpConnectionPool("www.sylar.top", "", 80, 10, 1000 * 30, 5));

    alotz::IOManager::GetThis()->addTimer(1000, [pool]() {
        auto r = pool->doGet("/", 300);
        ALOTZ_LOG_INFO(g_logger) << r->toString();
    }, true);
}

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

    ALOTZ_LOG_INFO(g_logger) << "===============";
    auto r = alotz::http::HttpConnection::doGet("http://www.baidu.com/blog/", 300);
    ALOTZ_LOG_INFO(g_logger) << "result=" << r->result
        << " error=" << r->error
        << " rsp=" << (r->response ? r->response->toString() : "");
    ALOTZ_LOG_INFO(g_logger) << "===============";
    test_pool();
}

int main(int argc, char** argv) {
    alotz::IOManager iom(2);
    iom.schedule(run);
    return 0;
}