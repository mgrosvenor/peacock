#ifndef PEACOCK_HOST_MSG
#define PEACOCK_HOST_MSG

#include "../msg/msg.h"

void init_host_msgs();

__attribute__((format(printf, 1, 2)))
void errorf(const char* fmt, ...);

int pck_get_response(msg_t* msg);
void pck_init_host_msgs();
int pck_get_errors();
int pck_success(const char n0, const char n1, const int pcount);

#endif // PEACOCK_HOST_MSG
