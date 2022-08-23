#include "peacock.h"
#include <peacock/peacock_serial.h>
#include <peacock/peacock_msg.h>
#include <peacock/peacock_lock.h>

#include <stdio.h>
#include <string.h>


int pck_init(const char* dev, bool force)
{
    const int res = pck_lock_init(force);
    if(res < 0)
    {
        return -1;
    }

    pck_init_host_msgs();

    int fd = pck_serial_open(dev);
    if (fd < 0)
    {
        return -1;
    }

    return 0;
}

void pck_close()
{        
    pck_serial_close();
    pck_lock_close();
}