#include "src/alotz.h"

alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

void run_in_fiber() {
    ALOTZ_LOG_INFO(g_logger) << "run in fiber begin";
    alotz::Fiber::YieldToHold();
    ALOTZ_LOG_INFO(g_logger) << "run in fiber end";
    alotz::Fiber::YieldToHold();
}

void test_fiber() {
    ALOTZ_LOG_INFO(g_logger) << "main begin -1";
    {
        alotz::Fiber::GetThis();
        ALOTZ_LOG_INFO(g_logger) << "main begin";
        alotz::Fiber::ptr fiber(new alotz::Fiber(run_in_fiber));
        fiber->swapIn();
        ALOTZ_LOG_INFO(g_logger) << "main after swapIn";
        fiber->swapIn();
        ALOTZ_LOG_INFO(g_logger) << "main after end";
        fiber->swapIn();
    }
    ALOTZ_LOG_INFO(g_logger) << "main after end";
}

int main(int argc, char** argv) {
    alotz::Thread::SetName("main");

    std::vector<alotz::Thread::ptr> thrs;
    for (int i = 0; i < 3; ++i) {
        thrs.push_back(alotz::Thread::ptr(new alotz::Thread(&test_fiber, "name-" + std::to_string(i))));
    }
    for (auto i : thrs) {
        i->join();
    }
    return 0;
}