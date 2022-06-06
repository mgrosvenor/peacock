#include "peacock_host_util.h"
#include "peacock_host_msg.h"

int pck_util_sleep(int us)
{    
    msg_t msg = {
        .name = { 'U', 's' },
            .pcount = 1,
            .params =
        {
            INT(us)
        }
    };

    send_msg(&msg);
    return 0;
}
