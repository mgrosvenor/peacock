#ifndef PEACOCK_HOST
#define PEACOCK_HOST

#include "peacock_host_gpio.h"
#include "peacock_host_pwm.h"
#include "peacock_host_util.h"
#include "peacock_host_msg.h"

int pck_host_init(const char* dev);
void pck_host_close();

#endif // PEACOCK_HOST
