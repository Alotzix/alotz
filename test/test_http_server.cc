#include "src/http/http_server.h"
#include "src/log.h"

static alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

#define XX(...) #__VA_ARGS__

alotz::IOManager::ptr worker;
void run() {
    g_logger->setLevel(alotz::LogLevel::INFO);
    alotz::http::HttpServer::ptr server(new alotz::http::HttpServer(true));
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

    sd->addGlobServlet("/sylarx/*", [](alotz::http::HttpRequest::ptr req
                , alotz::http::HttpResponse::ptr rsp
                , alotz::http::HttpSession::ptr session) {
            rsp->setBody(XX(<html>
<head><title>404 Not Found</title></head>
<body>
<center><h1>404 Not Found</h1></center>
<hr><center>nginx/1.16.0</center>
</body>
</html>
<!-- a padding to disable MSIE and Chrome friendly error page -->
<!-- a padding to disable MSIE and Chrome friendly error page -->
<!-- a padding to disable MSIE and Chrome friendly error page -->
<!-- a padding to disable MSIE and Chrome friendly error page -->
<!-- a padding to disable MSIE and Chrome friendly error page -->
<!-- a padding to disable MSIE and Chrome friendly error page -->
));
            return 0;
    });

    server->start();
}

int main(int argc, char** argv) {

    alotz::IOManager iom(1, true, "main");
    worker.reset(new alotz::IOManager(3, false, "worker"));
    iom.schedule(run);
    return 0;
}