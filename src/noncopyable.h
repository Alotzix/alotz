#ifndef __ALOTZ_NONCOPYABLE_H__
#define __ALOTZ_NONCOPYABLE_H__

namespace alotz {

class Noncopyable {
public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
};

}

#endif