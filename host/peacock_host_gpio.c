#include "peacock_host_gpio.h"
#include "peacock_host_msg.h"
#include "peacock_host_pins.h"
#include "peacock_host_err.h"



//Set output on GPIO pin
int pck_gpio_out(const int pin, const int val)
{
    if(!pin_valid(pin))
    {
        errorf("Invalid pin %i\n", pin);
        return -1;
    }

    if (val < -1 || val > 1)
    {
        errorf("Value %i should be in range -1 to +1\n", val);
        return -1;
    }

    const msg_t msg =
    {
        .name =  {'G', 'o'},
        .pcount = 2,
        .params =
        {
            INT(pin),
            INT(val),
        }
    };

    send_msg(&msg);
    return 0;
}


//Fetch input from a given GPIO pin
int pck_gpio_in(const int pin)
{
    if (!pin_valid(pin))
    {
        errorf("Invalid pin %i\n", pin);
        return -1;
    }

    msg_t msg =
    {
        .name = {'G', 'i'},
        .pcount = 2,
        .params =
        {
            INT(pin),
        }
    };

    send_msg(&msg);

    const int max_attempts = 100;
    for (int i = 0; get_response(&msg); i++)
    {
        if (i > max_attempts)
        {
            errorf("Made %i attempts to get response but found none\n", max_attempts);
            return -1;
        }
    }

    if (msg.name[0] != 'G' && msg.name[1] != 'i')
    {
        errorf("Unexpected reply message type. Expected 'Gi' but got '%c%c'\n", msg.name[0], msg.name[1]);
        return -1;
    }

    if (msg.pcount != 2)
    {
        errorf("Unexpected parameter count. Expected 2 but got %i\n", msg.pcount);
        return -1;
    }

    const int rx_pin = param_i(&msg, 0);
    const int rx_val = param_b(&msg, 1);

    if (rx_pin != pin)
    {
        errorf("Wrong pin numer! Expected %i but got %i\n", pin, rx_pin);
        return -1;
    }

    return rx_val;
}

//Set the pull ups/downs on the give pin
int pck_gpio_pull(int pin, bool up, bool dwn)
{
    if (!pin_valid(pin))
    {
        errorf("Invalid pin %i\n", pin);
        return -1;
    }

    const msg_t msg =
    {
        .name = {'G', 'p'},
        .pcount = 3,
        .params =
        {
            INT(pin),
            BOOL(up),
            BOOL(dwn),
        }
    };

    send_msg(&msg);
    return 0;
}


//Set the pin function 'g' for GPIO, 'p' for PWM.
int pck_gpio_pin_func(const int pin, const char func)
{
    if (!pin_valid(pin))
    {
        errorf("Invalid pin %i\n", pin);
        return -1;
    }

    if (func != 'g' && func != 'p')
    {
        errorf("Function '%c' should be in g=gpio or p=pwm\n", func);
        return -1;
    }

    const msg_t msg =
    {
        .name = {'G', 'f'},
        .pcount = 2,
        .params =
        {
            INT(pin),
            CHAR(func),
        }
    };

    send_msg(&msg);
    return 0;
}
