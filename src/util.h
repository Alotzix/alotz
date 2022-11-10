#ifndef ALOTZ_UTIL_H
#define ALOTZ_UTIL_H

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>

namespace alotz {
    
pid_t GetThreadId();
uint32_t GetFiberId();

void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");

uint64_t GetCurrentMS();
uint64_t GetCurrentUS();

std::string Time2Str(time_t ts = time(0), const std::string& format = "%Y-%m-%d %H:%M:%S");
time_t Str2Time(const char* str, const char* format = "%Y-%m-%d %H:%M:%S");

template<typename V, typename Map, typename K>
V GetParamValue(const Map& m, const K& k, const V& def = V()) {
    auto it = m.find(k);
    if (it == m.end()) {
        return def;
    }
    try {
        return boost::lexical_cast<V>(it->second);
    } catch (...) {
    }
    return def;
}

template<typename V, typename Map, typename K>
bool CheckGetParamValue(const Map& m, const K& k, V& v) {
    auto it = m.find(k);
    if (it == m.end()) {
        return false;
    }
    try {
        v = boost::lexical_cast<V>(it->second);
        return true;
    } catch (...) {
    }
    return false;
}

template<typename T>
void nop(T*) {}

template<typename T>
void delete_array(T* v) {
    if (v) {
        delete[] v;
    }
}

class TypeUtil {
public:
    static int8_t ToChar(const std::string& str);
    static int64_t Atoi(const std::string& str);
    static double Atof(const std::string& str);
    static int8_t ToChar(const char* str);
    static int64_t Atoi(const char* str);
    static double Atof(const char* str);
};

class StringUtil {
public:
    static std::string Format(const char* fmt, ...);
    static std::string Formatv(const char* fmt, va_list ap);


    static std::string Trim(const std::string& str, const std::string& delimit = "\t\r\n");
    static std::string TrimLeft(const std::string& str, const std::string& delimit = "\t\r\n");
    static std::string TrimRight(const std::string& str, const std::string& delimit = "\t\r\n");

    static std::string WStringToString(const std::wstring& ws);
    static std::string StringToWString(const std::string& s);
};

}

#endif