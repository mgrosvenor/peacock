#ifndef SERIAL
#define SERIAL

#include <stdarg.h>
#include <stdint.h>

int open_serial_port(const char* device);
int serial_getchar();
int serial_peek();

int serial_vsend_str(const char* fmt, va_list args);

#endif // SERIAL
