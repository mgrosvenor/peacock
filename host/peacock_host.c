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
    fprintf(stderr, "HostErr: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr,"\n");
    fflush(stderr);
    va_end(args);
}

__attribute__((format(printf, 1, 2)))
int sendf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int result = serial_vsend_str(fmt, args);
    va_end(args);
    
    // va_start(args, fmt);
    // fprintf(stderr, "Debug: sending: ");
    // vfprintf(stderr, fmt, args);
    // va_end(args);

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
            fprintf(stderr, "Device error: %s\n", msg.params[0].s);
            return 0;
        default:
            fprintf(stderr, "Unkown message type %c\n", msg.name);
            return -1 ;
    }
    
    return 0;
}

void process_responses()
{
    while (serial_peek())
    {
        response();
    }
}


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: serial DEV\n");
        return -1;
    }
    printf("Host starting\n");

    const char* device = argv[1];

    int fd = open_serial_port(device);
    if (fd < 0)
    {
        return 1;
    }

    process_responses();

    int sleep = 10*10000;
    for (int i = 0;; i++)
    {
        
        gpio_set(25, 1);
        usleep(sleep);
//        process_responses();
        
        gpio_set(25, -1);
        usleep(sleep);
//        process_responses();
    }

    close(fd);
    return 0;
}
