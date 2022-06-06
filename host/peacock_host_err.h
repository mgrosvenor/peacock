#ifndef PEACOCK_HOST_ERR
#define PEACOCK_HOST_ERR

#include <stdarg.h>

__attribute__((format(printf, 1, 2)))
void errorf(const char* fmt, ...);

#endif // PEACOCK_HOST_ERR
