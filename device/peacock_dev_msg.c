#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include "../msg/msg.h"

#include "peacock_dev_msg.h"

__attribute__((format(printf, 1, 2)))
static int sendf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int result = vprintf(fmt, args);
    va_end(args);
    fflush(stdout);
    return result;
}

__attribute__((format(printf, 1, 2)))
void errorf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int err_len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    char* err = calloc(1, err_len + 1);
    va_start(args, fmt);
    vsnprintf(err, err_len, fmt, args);
    va_end(args);

    msg_t msg =
    {
        .name = { 'E', 'E' },
        .pcount = 1,
        .params =
        {
            STR(err)
        }
    };

    send_msg(&msg);
}


void init_dev_msgs()
{
    msg_funcs_t msg_funcs =
    {
        .isdev   = true,
        .getchar = getchar,
        .sendf   = sendf,
        .errorf  = errorf,
    };

    init_msgs(&msg_funcs);
}

