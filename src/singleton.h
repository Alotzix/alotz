#ifndef __ALOTZ_SINGLETON_H__
#define __ALOTZ_SINGLETON_H__

#include <memory>

namespace alotz {

template<typename T>
class Singleton {
public:
    static T* GetInstance() {
        static T v;
        return &v;
    }
};

template<typename T>
class SingletonPtr {
public:
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> v(new T);
        return v;
    }
};

}

#endif