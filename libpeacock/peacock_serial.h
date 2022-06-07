#ifndef PEACOCK_SERIAL
#define PEACOCK_SERIAL

#include <stdarg.h>
#include <stdint.h>

int pck_serial_open(const char* device);
int pck_serial_getchar();
int pck_serial_peek();

int pck_serial_vsendf(const char* fmt, va_list args);
void pck_serial_close();

#endif // PEACOCK_SERIAL
