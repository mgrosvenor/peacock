#include "peacock_dev_pins.h"
#include "peacock_dev_msg.h"

#include <hardware/gpio.h>
#include <stdlib.h>

pin_state_t pins[PIN_COUNT] = {0};

pin_state_t* get_pin_state(const int pin)
{
    if (pin < 0 || pin > PIN_COUNT)
    {
        errorf("Pun number out of range [0,28]. Got %i\n", pin);
        return NULL;
    }

    if (pin >= 23 && pin <= 24)
    {
        errorf("I/O pin %i is not available for user use (23,24)\n", pin);
        return NULL;
    }

    return &pins[pin];
}


