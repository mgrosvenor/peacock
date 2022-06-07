#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../msg/msg.h"

#include "peacock_dev_err.h"


static inline void _errorf(bool debug, const char* fmt, va_list args)
{
    const int err_len = vsnprintf(NULL, 0, fmt, args);
    char* err = calloc(1, err_len + 1);
    vsnprintf(err, err_len, fmt, args);

    msg_t msg = debug ? INIT_MSG('D', 'D', 1) : INIT_MSG('E', 'E', 1);
    SET_MSG_PARAM_S(&msg, 0, err);
    send_msg(&msg);
    free(err);
}



__attribute__((format(printf, 1, 2)))
void errorf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _errorf(false, fmt, args);
    va_end(args);
}


__attribute__((format(printf, 1, 2)))
void debugf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _errorf(true, fmt, args);
    va_end(args);
}
