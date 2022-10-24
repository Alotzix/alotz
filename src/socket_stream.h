#ifndef __ALOTZ_SOCKET_STREAM_H__
#define __ALOTZ_SOCKET_STREAM_H__

#include "stream.h"
#include "socket.h"

namespace alotz {

class SocketStream : public Stream {
public:
    typedef std::shared_ptr<SocketStream> ptr;
    SocketStream(Socket::ptr sock, bool owner = true);
    ~SocketStream();

    virtual int read(void* buffer, size_t length) override;
    virtual int read(Bytearray::ptr ba, size_t length) override;
    virtual int write(void* buffer, size_t length) override;
    virtual int write(Bytearray::ptr ba, size_t length) override;
    virtual void close() override;

    Socket::ptr getSocket() const { return m_socket; }
    bool isConnected() const;

private:
    Socket::ptr m_socket;
    bool m_owner;
};

}

#endif