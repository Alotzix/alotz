#include "src/alotz.h"
#include <unistd.h>

alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

void func1() {
    ALOTZ_LOG_INFO(g_logger) << "name: " << alotz::Thread::GetName()
                            << " this.name: " << alotz::Thread::GetThis()->GetName()
                            << " id: " << alotz::GetThreadId()
                            << " this.id: " << alotz::Thread::GetThis()->getId();
    sleep(20);
}

void func2() {

}

int main(int argc, char** argv) {
    ALOTZ_LOG_INFO(g_logger) << "thread test begin";
    std::vector<alotz::Thread::ptr> thrs;
    for (int i = 0; i < 5; ++i) {
        alotz::Thread::ptr thr(new alotz::Thread(&func1, "name_" + std::to_string(i)));
        thrs.push_back(thr);
    }

    for (int i = 0; i < 5; ++i) {
        thrs[i] -> join();
    }

    ALOTZ_LOG_INFO(g_logger) << "thread test end.";

    return 0;
}