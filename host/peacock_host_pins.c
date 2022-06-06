#include "peacock_host_pins.h"
#include "peacock_host_err.h"

int pin_valid(int pin)
{
    if (pin < 0 || pin > PIN_COUNT-1)
    {
        errorf("I/O pin %i is out of range (0-%i)\n", pin, PIN_COUNT-1);
        return 0;
    }

    if (pin >= 23 && pin <= 24)
    {
        errorf("I/O pin %i is not available for user use (23,24)\n", pin);
        return 0;
    }

    return 1;
}