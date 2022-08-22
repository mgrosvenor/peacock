#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include <common/peacock_msg/peacock_msg.h>

#include <peacock/peacock_serial.h>
#include <peacock/peacock_err.h>
#include <peacock/peacock_lock.h>

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



static int _pck_success(msg_t* msg, const char n0, const char n1, const int pcount)
{
    //Try for 1000 messages, or give up
    for(int i = 0; i < 1000; i++)
    {
        if (get_msg(msg))
        {
            fprintf(stderr, "Could not get message response from device\n");
            return -1;
        }

        const char msg_n0 = msg->name[0];
        switch (msg_n0)
        {
        case 'E':
            fprintf(stderr, "Device error: %s\n", param_s(msg, 0));
            continue;
        case 'D':
            fprintf(stderr, "Device debug: %s\n", param_s(msg, 0));
            continue;
        }

       return is_msg_success(msg, n0, n1, pcount);
    }

    return -1;
}

// This function combines the send message function with the success function
// in a locked, threadsafe way. msg is both an input and an output parameter
int pck_do_msg_ts(msg_t* const msg, const char n0, const char n1, const int pcount)
{
    int err = pck_lock();
    if(err < 0)
    {
        return -1;
    }
    
    send_msg(msg);
    const int result = _pck_success(msg, n0, n1, pcount);
    
    err = pck_unlock();
    if(err < 0 )
    {
        return -1;
    }
    return result;
}