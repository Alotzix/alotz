#include "src/alotz.h"

static alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

void test_fiber() {
    ALOTZ_LOG_INFO(g_logger) << "test in fiber";
}

int main(int argc, char** argv) {
    ALOTZ_LOG_INFO(g_logger) << "main";
    alotz::Scheduler sc;
    sc.schedule(&test_fiber);
    sc.start();
    ALOTZ_LOG_INFO(g_logger) << "schedule";
    sc.stop();
    ALOTZ_LOG_INFO(g_logger) << "over";
    return 0;
}