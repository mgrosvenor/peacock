#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include <peacock_msg/peacock_msg.h>
#include <peacock_dev_msg.h>
#include <peacock_dev_err.h>

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


void init_dev_msgs()
{
    msg_funcs_t msg_funcs =
    {
        .isdev   = true,
        .getchar = getchar,
        .sendf   = sendf,
        .errorf  = errorf,
        .debugf  = debugf
    };

    init_msgs(&msg_funcs);
}

