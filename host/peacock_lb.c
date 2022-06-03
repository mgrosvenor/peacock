#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "serial.h"


__attribute__((format(printf, 1, 2)))
int sendf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int result = serial_vsend_str(fmt, args);    
    va_end(args);
    
    va_start(args, fmt);
    vprintf(fmt, args);
    fflush(stdout);
    va_end(args);
    
    return result;
}

static inline void printall()
{   
    while(1)
    {
        if (!serial_peek())
        {
            break;
        }
        int c = serial_getchar();
        printf("0x%02X %c\n", c, isalnum(c) ? c : '.');
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

    printall();
    
    for(int i = 0; ; i++)
    {                                
        sendf("%i\n", i);
        printall();
    }

    close(fd);
    return 0;
}
