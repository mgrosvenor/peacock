#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include <common/peacock_msg/peacock_msg.h>

#include <peacock/peacock_serial.h>
#include <peacock/peacock_err.h>

//static int nl = true;

__attribute__((format(printf, 1, 2)))
int sendf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int result = pck_serial_vsendf(fmt, args);
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
        .getchar = pck_serial_getchar,
        .sendf = sendf,
        .errorf = errorf,
        .debugf = debugf,
    };

    init_msgs(&msg_funcs);
}



//Work through list of responses until we find a non error response, or nothing
int pck_next_msg(msg_t* msg)
{
    while (1)
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
        case 'D':
            fprintf(stderr, "Device debug: %s\n", param_s(msg, 0));
            continue;
        default:
            return 0;
        }
    }

    //Unreachable
    return -1;
}


int pck_success(const char n0, const char n1, const int pcount)
{
    while(1)
    {
        msg_t msg = {0};
        if (get_msg(&msg))
        {
            fprintf(stderr, "Could not get message response from device\n");
            return -1;
        }

        const char msg_n0 = msg.name[0];
        switch (msg_n0)
        {
        case 'E':
            fprintf(stderr, "Device error: %s\n", param_s(&msg, 0));
            continue;
        case 'D':
            fprintf(stderr, "Device debug: %s\n", param_s(&msg, 0));
            continue;
        }

       return is_msg_success(&msg, n0, n1, pcount);
    }
}