#include "peacock_host_err.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

static inline void _errorf(bool debug, const char* fmt, va_list args)
{
    fprintf(stderr, "Host %s: ", debug ? "debug" : "error");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    fflush(stderr);
}

__attribute__((format(printf, 1, 2)))
void errorf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _errorf(false,fmt, args);
    va_end(args);
}

__attribute__((format(printf, 1, 2)))
void debugf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _errorf(true,fmt, args);
    va_end(args);
}
