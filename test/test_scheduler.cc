#include "src/alotz.h"

static alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

void test_fiber() {
    static int s_count = 5;
    ALOTZ_LOG_INFO(g_logger) << "test in fiber s_count= " << s_count;

    sleep(1);
    if (--s_count >= 0) {
        alotz::Scheduler::GetThis()->schedule(&test_fiber, alotz::GetThreadId());
    }
}

int main(int argc, char** argv) {
    ALOTZ_LOG_INFO(g_logger) << "main";
    alotz::Scheduler sc(3, false, "test");
    sc.start();
    sleep(2);
    ALOTZ_LOG_INFO(g_logger) << "schedule";
    sc.schedule(&test_fiber);
    sc.stop();
    ALOTZ_LOG_INFO(g_logger) << "over";
    return 0;
}