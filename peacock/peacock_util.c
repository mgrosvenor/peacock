#include <peacock/peacock_util.h>
#include <peacock/peacock_msg.h>

int pck_util_sleep(int us)
{    
    const char n0 = 'U'; //Util group
    const char n1 = 's'; //Sleep function

    msg_t msg = INIT_MSG(n0, n1, 1);
    SET_MSG_PARAM_I(&msg, 0, us);
    return pck_do_msg_ts(&msg, n0, n1, 0);

}
