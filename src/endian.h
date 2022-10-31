#ifndef ALOTZ_ENDIAN_H
#define ALOTZ_ENDIAN_H

#define ALOTZ_LITTLE_ENDIAN 1
#define ALOTZ_BIG_ENDIAN 2

#include <byteswap.h>
#include <stdint.h>

namespace alotz {

template <typename T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
byteswap(T value) {
    return (T)bswap_64((uint64_t)value);
}

template <typename T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
byteswap(T value) {
    return (T)bswap_32((uint32_t)value);
}

template <typename T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
byteswap(T value) {
    return (T)bswap_16((uint16_t)value);
}

#if BYTE_ORDER == BIG_ENDIAN
#define ALOTZ_BYTE_ORDER ALOTZ_BIG_ENDIAN
#else
#define ALOTZ_BYTE_ORDER ALOTZ_LITTLE_ENDIAN
#endif

#if ALOTZ_BYTE_ORDER == ALOTZ_BIG_ENDIAN
template<typename T>
T byteswapOnLittleEndian(T t) {
    return t;
}

template<class T>
T byteswapOnBigEndian(T t) {
    return byteswap(t);
}

#else

template<class T>
T byteswapOnLittleEndian(T t) {
    return byteswap(t);
}

template<class T>
T byteswapOnBigEndian(T t) {
    return t;
}

#endif

}

#endif