#include <execinfo.h>
#include <sys/time.h>
#include "util.h"
#include "log.h"
#include "fiber.h"

namespace alotz {

static alotz::Logger::ptr g_logger = ALOTZ_LOG_NAME("system");

pid_t GetThreadId() {
    return syscall(SYS_gettid);
}

uint32_t GetFiberId() {
    return alotz::Fiber::GetFiberId();
}

void Backtrace(std::vector<std::string>& bt, int size, int skip) {
    void** array = (void**)malloc(sizeof(void*) * size);
    size_t s = ::backtrace(array, size);

    char** strings = backtrace_symbols(array, s);
    if (strings == NULL) {
        ALOTZ_LOG_ERROR(g_logger) << "backtrace_symbols error";
        return;
    }

    for (size_t i = skip; i < s; ++i) {
        bt.push_back(strings[i]);
    }

    free(strings);
    free(array);
}

std::string BacktraceToString(int size, int skip, const std::string& prefix) {
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);

    std::stringstream ss;
    for (size_t i = 0;  i < bt.size(); ++i) {
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}

uint64_t GetCurrentMS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}

uint64_t GetCurrentUS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 * 1000ul + tv.tv_usec;
}

std::string Time2Str(time_t ts, const std::string& format) {
    struct tm tm;
    localtime_r(&ts, &tm);
    char buf[64];
    strftime(buf, sizeof(buf), format.c_str(), &tm);
    return buf;
}

time_t Str2Time(const char* str, const char* format) {
    struct tm t;
    memset(&t, 0, sizeof(t));
    if (!strptime(str, format, &t)) {
        return 0;
    }
    return mktime(&t);
}

int8_t TypeUtil::ToChar(const std::string& str) {
    if (str.empty()) {
        return 0;
    }
    return *str.begin();
}

int64_t TypeUtil::Atoi(const std::string& str) {
    if (str.empty()) {
        return 0;
    }
    return strtoull(str.c_str(), nullptr, 10);
}

double  TypeUtil::Atof(const std::string& str) {
    if(str.empty()) {
        return 0;
    }
    return atof(str.c_str());
}

int8_t  TypeUtil::ToChar(const char* str) {
    if(str == nullptr) {
        return 0;
    }
    return str[0];
}

int64_t TypeUtil::Atoi(const char* str) {
    if(str == nullptr) {
        return 0;
    }
    return strtoull(str, nullptr, 10);
}

double  TypeUtil::Atof(const char* str) {
    if(str == nullptr) {
        return 0;
    }
    return atof(str);
}

std::string StringUtil::Format(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    auto v = Formatv(fmt, ap);
    va_end(ap);
    return v;
}

std::string StringUtil::Formatv(const char* fmt, va_list ap) {
    char* buf = nullptr;
    auto len = vasprintf(&buf, fmt, ap);
    if(len == -1) {
        return "";
    }
    std::string ret(buf, len);
    free(buf);
    return ret;
}

std::string StringUtil::Trim(const std::string& str, const std::string& delimit) {
    auto begin = str.find_first_not_of(delimit);
    if (begin == std::string::npos) {
        return "";
    }
    auto end = str.find_last_not_of(delimit);
    return str.substr(begin, end - begin + 1);
}

std::string StringUtil::TrimLeft(const std::string& str, const std::string& delimit) {
    auto begin = str.find_first_not_of(delimit);
    if (begin == std::string::npos) {
        return "";
    }
    return str.substr(begin);
}

std::string StringUtil::TrimRight(const std::string& str, const std::string& delimit) {
    auto end = str.find_last_not_of(delimit);
    if (end == std::string::npos) {
        return "";
    }
    return str.substr(0, end);
}

}