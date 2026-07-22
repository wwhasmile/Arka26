#include "log.h"
#include "assert.h"

#include <sys/sys.h>

#include <stdarg.h>
#include <stdio.h>

void Log_Output(logLevel_t level, const char *fmt, ...)
{
    const static char* logLevelStrings[] = { "INFO", "SUCCESS", "DEBUG", "WARNING", "ERROR", "FATAL" };

    char message[4096] = { 0 };
    __builtin_va_list args;
    va_start(args, fmt);
    vsnprintf(message, 4096, fmt, args);
    va_end(args);

    char output[4096] = { 0 };
    sprintf(output, "[%s]: %s", logLevelStrings[level], message);
    Sys_Print(output, level);
}

#ifndef NDEBUG
void Assert_Report(const char* expr, const char* msg, const char* file, u64 line)
{
    if (msg == NULL)
        Log_Output(LOG_LEVEL_FATAL, "Assertion failed: %s in %s:%llu", expr, file, line);
    else
        Log_Output(LOG_LEVEL_FATAL, "Assertion failed: %s - %s in %s:%llu", expr, msg, file, line);
}
#endif
