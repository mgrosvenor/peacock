#include <peacock/peacock.h>
#include <peacock/peacock_serial.h>
#include <peacock/peacock_msg.h>

int pck_init(const char* dev)
{
    
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
}