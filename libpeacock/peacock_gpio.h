#ifndef PEACOCK_HOST_GPIO
#define PEACOCK_HOST_GPIO

#include <stdbool.h>

//Fetch input from a given GPIO pin
int pck_gpio_in(const int pin);

//Set output on GPIO pin
int pck_gpio_out(const int pin, const int val);

//Set the pull ups/downs on the give pin
int pck_gpio_pull(int pin, bool up, bool dwn);

//Set the pin function 'g' for GPIO, 'p' for PWM.
int pck_gpio_pin_func(const int pin, const char func);

#define PCK_GPIO_FUNC_PWM 'p'
#define PCK_GPIO_FUNC_GPIO 'g'

#endif // PEACOCK_HOST_GPIO
