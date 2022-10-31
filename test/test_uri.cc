#include "src/uri.h"
#include <iostream>

int main(int argc, char** argv) {
    alotz::Uri::ptr uri = alotz::Uri::Create("http://www.baidu.com");
    std::cout << uri->toString() << std::endl;
    auto addr = uri->createAddress();
    std::cout << *addr << std::endl;
    return 0;
}