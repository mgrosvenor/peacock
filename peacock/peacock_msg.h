#ifndef PEACOCK_MSG
#define PEACOCK_MSG

#include <common/peacock_msg/peacock_msg.h>

void init_host_msgs();

__attribute__((format(printf, 1, 2)))
void errorf(const char* fmt, ...);


void pck_init_host_msgs();
int pck_next_msg(msg_t* msg);
int pck_success(const char n0, const char n1, const int pcount);

#endif // PEACOCK_MSG
