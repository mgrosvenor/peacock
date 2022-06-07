#ifndef PEACOCK
#define PEACOCK

#include <peacock/peacock_gpio.h>
#include <peacock/peacock_pwm.h>
#include <peacock/peacock_util.h>

int pck_init(const char* dev);
void pck_close();

#endif // PEACOCK
