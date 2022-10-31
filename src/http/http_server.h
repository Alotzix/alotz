#ifndef ALOTZ_HTTP_HTTP_SERVER_H
#define ALOTZ_HTTP_HTTP_SERVER_H

#include "src/tcp_server.h"
#include "http_session.h"
#include "servlet.h"

namespace alotz {
namespace http {

class HttpServer : public TcpServer {
public:
    typedef std::shared_ptr<HttpServer> ptr;
    HttpServer(bool keepalive = false,
        alotz::IOManager* worker = alotz::IOManager::GetThis(),
        alotz::IOManager* accept_worker = alotz::IOManager::GetThis());
    
    ServletDispatch::ptr getServletDispatch() const { return m_dispatch; }
    void setServletDispatch(ServletDispatch::ptr v) { m_dispatch = v; }
protected:
    virtual void handleClient(Socket::ptr client) override;
private:
    bool m_isKeepalive;
    ServletDispatch::ptr m_dispatch;
};

}
}

#endif