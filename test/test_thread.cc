#include "src/alotz.h"
#include <unistd.h>

alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

int count = 0;
// alotz::RWMutex s_mutex;
alotz::Mutex s_mutex;

void func1() {
    ALOTZ_LOG_INFO(g_logger) << "name: " << alotz::Thread::GetName()
                            << " this.name: " << alotz::Thread::GetThis()->GetName()
                            << " id: " << alotz::GetThreadId()
                            << " this.id: " << alotz::Thread::GetThis()->getId();
    for (int i = 0; i < 100000; ++i) {
        // alotz::RWMutex::WriteLock lock(s_mutex);
        alotz::Mutex::Lock lock(s_mutex);
        ++count;
    }
}

void func2() {
    while (true) {
        ALOTZ_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void func3() {
    while (true) {
        ALOTZ_LOG_INFO(g_logger) << "===============================";
    }
}

int main(int argc, char** argv) {
    ALOTZ_LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("/home/lighthouse/workspace/alotz/bin/conf/test.yml");
    alotz::Config::LoadFromYaml(root);

    std::vector<alotz::Thread::ptr> thrs;
    for (int i = 0; i < 2; ++i) {
        alotz::Thread::ptr thr(new alotz::Thread(&func2, "name_" + std::to_string(i * 2)));
        alotz::Thread::ptr thr2(new alotz::Thread(&func3, "name_" + std::to_string(i * 2 + 1)));
        thrs.push_back(thr);
        thrs.push_back(thr2);
    }

    for (size_t i = 0; i < thrs.size(); ++i) {
        thrs[i] -> join();
    }

    ALOTZ_LOG_INFO(g_logger) << "thread test end.";
    ALOTZ_LOG_INFO(g_logger) << "count = " << count;

    return 0;
}