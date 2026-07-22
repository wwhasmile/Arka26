#ifndef __LOG_H__
#define __LOG_H__

#include <core/defines.h>

ENUM(logLevel_t, u8)
{
    LOG_LEVEL_INFO,
    LOG_LEVEL_SUCCESS,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
};

void Log_Output(logLevel_t level, const char* fmt, ...);

#define LOG_INFO(fmt, ...) Log_Output(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define LOG_SUCCESS(fmt, ...) Log_Output(LOG_LEVEL_SUCCESS, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) Log_Output(LOG_LEVEL_WARNING, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) Log_Output(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) Log_Output(LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)

#ifndef NDEBUG
#define LOG_DEBUG(fmt, ...) Log_Output(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif

#endif // __LOG_H__
