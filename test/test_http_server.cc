#include "src/http/http_server.h"
#include "src/log.h"

static alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

void run() {
    alotz::http::HttpServer::ptr server(new alotz::http::HttpServer);
    alotz::Address::ptr addr = alotz::Address::LookupAnyIPAddress("0.0.0.0:8020");
    while (!server->bind(addr)) {
        sleep(2);
    }

    auto sd = server->getServletDispatch();
    sd->addServlet("/alotz/xx", [](alotz::http::HttpRequest::ptr req
        , alotz::http::HttpResponse::ptr rsp
        , alotz::http::HttpSession::ptr session){
            rsp->setBody(req->toString());
            return 0;
        });

    sd->addGlobServlet("/alotz/*", [](alotz::http::HttpRequest::ptr req
        , alotz::http::HttpResponse::ptr rsp
        , alotz::http::HttpSession::ptr session) {
            rsp->setBody("Glob:\r\n" + req->toString());
            return 0;
    });

    server->start();
}

int main(int argc, char** argv) {

    alotz::IOManager iom(2);
    iom.schedule(run);
    return 0;
}