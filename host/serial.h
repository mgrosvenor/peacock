#ifndef SERIAL
#define SERIAL

int open_serial_port(const char* device);
int serial_getchar();

int serial_vsend_str(const char* fmt, va_list args);

#endif // SERIAL
