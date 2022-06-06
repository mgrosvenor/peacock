#include "peacock_host_err.h"

#include <stdio.h>

__attribute__((format(printf, 1, 2)))
void errorf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Host error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    fflush(stderr);
    va_end(args);
}
