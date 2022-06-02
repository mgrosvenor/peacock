#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "serial.h"
#include "../msg/msg.h"

int getchar_()
{
    return serial_getchar();
}

__attribute__((format(printf, 1, 2)))
void error(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    printf("\n");
    fflush(stderr);
    va_end(args);
}

__attribute__((format(printf, 1, 2)))
int send_str(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt,args);    
    const int result = serial_vsend_str(fmt, args);
    va_end(args);
    return result;
}

static inline int gpio_set(int pin, int val)
{
    if (pin < 0 || pin > 28)
    {
        fprintf(stderr, "I/O pin %i is out of range (0-28)\n", pin);
        return -1;
    }

    if (pin >= 23 && pin <= 24)
    {
        fprintf(stderr, "I/O pin %i is not available for user use (23,24)\n", pin);
        return -1;
    }

    if (val < -1 || val > 1)
    {
        fprintf(stderr, "Value %i should be in range -1 to +1\n", val);
        return -1;
    }

    const msg_t msg = 
    {
        .name = 'S',
        .pcount = 2,
        .params = 
        {
            { .type = 'i' , .i = pin },
            { .type = 'i' , .i = val }
        }
    };

    return send_msg(&msg);
}

int response()
{
    msg_t msg = { 0 };
    if(get_msg(&msg))
    {
        fprintf(stderr, "Could not get message response\n");
        return -1;
    }

    switch(msg.name)
    {
        case 'E': 
            fprintf(stderr, "Device reported error: %s", msg.params[0].s);
            return 0;
        default:
            fprintf(stderr, "Unkown message type %c\n", msg.name);
            return -1 ;
    }
    
    return 0;
}


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: serial DEV\n");
        return -1;
    }
    printf("starting\n");

    const char* device = argv[1];

    int fd = open_serial_port(device);
    if (fd < 0)
    {
        return 1;
    }

    for (int i = 0;; i++)
    {
        
        gpio_set(25, 1);
        usleep(500000);
        if(response()){
            return 1;
        }
        

        gpio_set(25, 0);
        usleep(500000);
        response();
    }

    close(fd);
    return 0;
}
