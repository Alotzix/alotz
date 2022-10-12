#include "src/alotz.h"

static alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

int main(int argc, char** argv) {
    alotz::Scheduler sc;
    sc.start();
    sc.stop();
    return 0;
}