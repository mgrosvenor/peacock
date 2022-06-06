#ifndef PEACOCK_DEV_MSG
#define PEACOCK_DEV_MSG

__attribute__((format(printf, 1, 2)))
void errorf(const char* fmt, ...);

void init_dev_msgs();

#endif // PEACOCK_DEV_MSG
