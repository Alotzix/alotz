#ifndef ALOTZ_SINGLETON_H
#define ALOTZ_SINGLETON_H

#include <memory>

namespace alotz {

template <typename T>
class Singleton {
public:
    static T* GetInstance() {
        static T v;
        return &v;
    }
};

template <typename T>
class SingletonPtr {
public:
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> v(new T);
        return v;
    }
};

}

#endif