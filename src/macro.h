#ifndef __ALOTZ_MACRO_H__
#define __ALOTZ_MACRO_H__

#include <string.h>
#include <assert.h>
#include "util.h"

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