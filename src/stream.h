#ifndef __ALOTZ_STREAM_H__
#define __ALOTZ_STREAM_H__

#include <memory>
#include "bytearray.h"

namespace alotz {

class Stream {
public:
    typedef std::shared_ptr<Stream> ptr;
    virtual ~Stream();
    
    virtual int read(void* buffer, size_t length) = 0;
    virtual int read(Bytearray::ptr ba, size_t length) = 0;
    virtual int readFixSize(void* buffer, size_t length) = 0;
    virtual int readFixSize(Bytearray::ptr ba, size_t length) = 0;
    virtual int write(const void* buffer, size_t length) = 0;
    virtual int write(Bytearray::ptr ba, size_t length) = 0;
    virtual int writeFixSize(const void* buffer, size_t length) = 0;
    virtual int writeFixSize(Bytearray::ptr ba, size_t length) = 0;
    virtual void close() = 0;
};

}

#endif