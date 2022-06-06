#include "peacock_host.h"
#include "serial.h"

int pck_host_init(const char* dev)
{
    
    pck_init_host_msgs();

    int fd = open_serial_port(dev);
    if (fd < 0)
    {
        return -1;
    }

    return 0;
}

void pck_host_close()
{
    serial_close();
}