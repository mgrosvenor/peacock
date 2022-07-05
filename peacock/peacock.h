#ifndef PEACOCK
#define PEACOCK

#ifdef __cplusplus
extern "C" {
#endif


#include <peacock/peacock_msg.h>
#include <peacock/peacock_serial.h>
#include <peacock/peacock_gpio.h>
#include <peacock/peacock_pins.h>
#include <peacock/peacock_serial.h>
#include <peacock/peacock_pwm.h>
#include <peacock/peacock_util.h>

int pck_init(const char* dev);
void pck_close();

#ifdef __cplusplus
}
#endif


#endif // PEACOCK
