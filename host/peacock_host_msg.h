#ifndef PEACOCK_HOST_MSG
#define PEACOCK_HOST_MSG

#include "../msg/msg.h"

void init_host_msgs();

__attribute__((format(printf, 1, 2)))
void errorf(const char* fmt, ...);

int get_response(msg_t* msg);
void pck_init_host_msgs();
int pck_get_errors();


#endif // PEACOCK_HOST_MSG
