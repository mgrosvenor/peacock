#ifndef PEACOCK_DEV_GPIO
#define PEACOCK_DEV_GPIO

#include "peacock_dev_pins.h"

void pck_gpio_init_all();
void pck_gpio_pull_disable_all();

//Fetch input from a given GPIO pin
int pck_gpio_in(const int pin);

//Set output on GPIO pin
int pck_gpio_out(const int pin, const int val);

//Set the pull ups/downs on the give pin
int pck_gpio_pull(int pin, bool up, bool dwn);

//Set the pin function 'g' for GPIO, 'p' for PWM.
int pck_gpio_pin_func(const int pin, const char func);


#endif // PEACOCK_DEV_GPIO
