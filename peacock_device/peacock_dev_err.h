#ifndef PEACOCK_DEV_ERR
#define PEACOCK_DEV_ERR

#include <stdarg.h>
#include <stdlib.h>

__attribute__((format(printf, 1, 2)))
void errorf(const char* fmt, ...);

__attribute__((format(printf, 1, 2)))
void debugf(const char* fmt, ...);

#endif // PEACOCK_DEV_ERR
