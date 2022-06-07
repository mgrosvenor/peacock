#ifndef PEACOCK_DEV_GPIO
#define PEACOCK_DEV_GPIO

#include <peacock_dev_pins.h>
#include <peacock_msg/peacock_msg.h>

void pck_gpio_init_all();
void pck_gpio_pull_disable_all();

//Fetch input from a given GPIO pin
msg_t pck_gpio_in(const msg_t* const msg, const char n0, const char n1);

//Set output on GPIO pin
msg_t pck_gpio_out(const msg_t* const msg, const char n0, const char n1);

//Set the pull ups/downs on the give pin
msg_t pck_gpio_pull(const msg_t* const msg, const char n0, const char n1);

//Set the pin function 'g' for GPIO, 'p' for PWM.
msg_t pck_gpio_pin_func(const msg_t* const msg, const char n0, const char n1);


#endif // PEACOCK_DEV_GPIO
