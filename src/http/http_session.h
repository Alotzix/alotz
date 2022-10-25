#ifndef __ALOTZ_HTTP_SESSION_H__
#define __ALOTZ_HTTP_SESSION_H__

#include "src/socket_stream.h"
#include "http.h"

namespace alotz {
namespace http {

class HttpSession : public SocketStream {
public:
    typedef std::shared_ptr<HttpSession> ptr;
    HttpSession(Socket::ptr sock, bool owner = true);
    HttpRequest::ptr recvRequest();
    int sendResponse(HttpResponse::ptr rsp);
};

}

}

#endif