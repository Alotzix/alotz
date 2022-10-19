#include "src/address.h"
#include "src/log.h"

alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

void test() {
    std::vector<alotz::Address::ptr> addrs;

    ALOTZ_LOG_INFO(g_logger) << "begin";
    bool v = alotz::Address::Lookup(addrs, "www.baidu.com", AF_INET);
    ALOTZ_LOG_INFO(g_logger) << "end";
    if (!v) {
        ALOTZ_LOG_ERROR(g_logger) << "lookup fail";
        return;
    }

    for (size_t i = 0; i < addrs.size(); ++i) {
        ALOTZ_LOG_INFO(g_logger) << i << " - " << addrs[i]->toString();
    }
}

void test_iface() {
    std::multimap<std::string, std::pair<alotz::Address::ptr, uint32_t>> results;

    bool v = alotz::Address::GetInterfaceAddress(results);
    if (!v) {
        ALOTZ_LOG_ERROR(g_logger) << "GetInterfaceAddress fail";
        return;
    }

    for (auto& i : results) {
        ALOTZ_LOG_INFO(g_logger) << i.first << " - " << i.second.first->toString() << " - "
            << i.second.second;
    }
}

void testipv4() {
    auto addr = alotz::IPAddress::Create("127.0.0.8");
    if (addr) {
        ALOTZ_LOG_INFO(g_logger) << addr->toString();
    }
}

int main(int argc, char** argv) {
    // test_ipv4();
    // test_iface();
    test();
    return 0;
}