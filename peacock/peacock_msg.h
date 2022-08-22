#ifndef PEACOCK_MSG
#define PEACOCK_MSG

#include <common/peacock_msg/peacock_msg.h>

void init_host_msgs();

__attribute__((format(printf, 1, 2)))
void errorf(const char* fmt, ...);

void pck_init_host_msgs();

// This function combines the send message function with the success function
// in a locked, threadsafe way. msg is both an input and an output parameter
int pck_do_msg_ts(const msg_t* const msg, const char n0, const char n1, const int pcount);

#endif // PEACOCK_MSG
