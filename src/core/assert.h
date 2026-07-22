#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <core/defines.h>

#ifndef NDEBUG
#ifdef _MSC_VER
#include <intrin.h>
#define debugBreak() __debugBreak()
#else
#define debugBreak() __builtin_trap()
#endif // _MSC_VER
#define ASSERT(x) \
if (x) \
{ \
} \
else \
{ \
    void Assert_Report(const char* expr, const char* msg, const char* file, u64 line); \
    Assert_Report(#x, "", __FILE__, __LINE__); \
    debugBreak(); \
}
#define ASSERT_MESSAGE(x, message) \
if (x) \
{ \
} \
else \
{ \
    void Assert_Report(const char* expr, const char* msg, const char* file, u64 line); \
    Assert_Report(#x, message, __FILE__, __LINE__); \
    debugBreak(); \
}
#else
#define ASSERT(x)
#define ASSERT_MESSAGE(x, message)
#endif // NDEBUG

#endif // __ASSERT_H__
