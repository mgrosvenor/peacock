#ifndef PEACOCK_ERR
#define PEACOCK_ERR

#include <stdarg.h>

__attribute__((format(printf, 1, 2)))
void errorf(const char* fmt, ...);

__attribute__((format(printf, 1, 2)))
void debugf(const char* fmt, ...);

#endif // PEACOCK_ERR
