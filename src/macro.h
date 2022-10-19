#ifndef __ALOTZ_MACRO_H__
#define __ALOTZ_MACRO_H__

#include <string.h>
#include <assert.h>
#include "util.h"

#if defined __GNUC__ || defined __llvm__
#   define ALOTZ_LIKELY(x) __builtin_expect(!!(x), 1)
#   define ALOTZ_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#   define ALOTZ_LIKELY(x) (x)
#   define ALOTZ_UNLIKELY(x) (x)
#endif 

#define ALOTZ_ASSERT(x) \
    if (!(x)) { \
        ALOTZ_LOG_ERROR(ALOTZ_LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace: \n" \
            << alotz::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#define ALOTZ_ASSERT2(x, w) \
    if (!(x)) { \
        ALOTZ_LOG_ERROR(ALOTZ_LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nBacktrace: \n" \
            << alotz::BacktraceToString(100, 2, "     "); \
        assert(x); \
    }

#endif