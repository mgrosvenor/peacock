#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "serial.h"

#include "../msg/msg.h"
#include "peacock_host_err.h"

//static int nl = true;

__attribute__((format(printf, 1, 2)))
int sendf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int result = serial_vsend_str(fmt, args);
    va_end(args);

    // va_start(args, fmt);
    // if(nl)
    // {
    //     fprintf(stderr, "Host debug: Sending: ");
    //     nl = false;
    // }
    // else
    // {
    //     for(const char* c = fmt; *c; c++)
    //     {
    //         if(*c == '\n')
    //         {
    //             nl = true;
    //             break;
    //         }
    //     }
    // }
    // vfprintf(stderr, fmt, args);
    // va_end(args);

    return result;
}


void pck_init_host_msgs()
{
    msg_funcs_t msg_funcs =
    {
        .getchar = serial_getchar,
        .sendf = sendf,
        .errorf = errorf
    };

    init_msgs(&msg_funcs);
}


//Work through list of responses until we find a non error response, or nothing
int get_response(msg_t* msg)
{
    while (serial_peek())
    {
        if (get_msg(msg))
        {
            fprintf(stderr, "Could not get message response\n");
            return -1;
        }

        switch (msg->name[0])
        {
        case 'E':
            fprintf(stderr, "Device error: %s\n", param_s(msg, 0));
            continue;
        default:
            return 0 ;
        }
    }

    return 1;
}

//Work through list of responses until we find a non error response, or nothing
int pck_get_errors()
{
    msg_t msg = {0};
    while (serial_peek())
    {
        if (get_msg(&msg))
        {
            fprintf(stderr, "Could not get message response\n");
            return -1;
        }

        switch (msg.name[0])
        {
        case 'E':
            fprintf(stderr, "Device error: %s\n", param_s(&msg, 0));
            continue;
        default:
            return 0;
        }
    }

    return 1;
}


