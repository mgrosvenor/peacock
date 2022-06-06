#include "hardware/gpio.h"

#include "peacock_dev_gpio.h"
#include "peacock_dev_msg.h"

int pck_gpio_pin_func(const int pin, char func);

static inline void _gpio_init(const int pin, pin_state_t* pin_state)
{
    pin_state->gpio.out_en = false;
    pin_state->gpio.out = 0;
    
    pck_gpio_pin_func((uint)pin, 'g');
    gpio_set_dir((uint)pin, GPIO_IN);
}

void pck_gpio_init_all()
{
    for (int i = 0; i < PIN_COUNT; i++)
    {
        if(i == 23 || i == 24)
        {
            //Skip invalid pins
            continue;
        }
        
        pin_state_t* pin_state = &pins[i];
        _gpio_init(i, pin_state);
    }
}


static inline void _pull_disable(int pin)
{
    //No error checking here because this is internal
    pin_state_t* pin_state = &pins[pin];    
    pin_state->pull.dwn = false;
    pin_state->pull.up = false;
    gpio_disable_pulls(pin);
}

void pck_gpio_pull_disable_all()
{
    for (int i = 0; i < PIN_COUNT; i++)
    {
        if (i == 23 || i == 24)
        {
            //Skip invalid pins
            continue;
        }
        _pull_disable(i);
    }
}


//Fetch input from a given GPIO pin
int pck_gpio_in(const int pin)
{
    pin_state_t* pin_state = get_pin_state(pin);
    if(!pin_state)
    {
        errorf("GPIO pin input not get pin state\n");
        return -1;
    }

    if (pin_state->func != 'g')
    {
        errorf("Pin is not configured for GPIO\n");
        return -1;
    }

    if (pin_state->gpio.out_en)
    {
        gpio_set_dir((uint)pin, GPIO_IN);
        pin_state->gpio.out_en = false;
    }

    const bool val = gpio_get((uint)pin);
    return val;
}

//Set output on GPIO pin
int pck_gpio_out(const int pin, const int val)
{
    pin_state_t* pin_state = get_pin_state(pin);
    if(!pin_state)
    {
        errorf("Invalid pin for output");
        return -1;
    }

    if( pin_state->func != 'g')
    {
        errorf("Pin is not configured for GPIO\n");
        return -1;
    }

    //Don't unnecessarily drive the pins
    if (pin_state->gpio.out == val)
    {
        return 0;
    }

    if(val < -1 || val > 1)
    {
        errorf("Value value out of range [-1,0,1]. Got %i\n", val);
        return -1;
    }

    if (val == -1)
    {
        gpio_set_dir((uint)pin, GPIO_IN);
        pin_state->gpio.out    = -1;
        pin_state->gpio.out_en = false;
        return 0;
    }
    else
    {
        if(!pin_state->gpio.out_en)
        {
            gpio_set_dir((uint)pin, GPIO_OUT);
        }
        gpio_put((uint)pin, (int)val);
        pin_state->gpio.out    = val;
        pin_state->gpio.out_en = true;
    }
    return 0;
}

int pck_gpio_pull(int pin, bool up, bool dwn)
{
    pin_state_t* pin_state = get_pin_state(pin);
    if (!pin_state)
    {
        errorf("Invalid pin for pull up/down");
        return -1;
    }

    pin_state->pull.dwn = dwn;
    pin_state->pull.up = up;
    gpio_set_pulls(pin, up, dwn);
    return 0;
}

int pck_gpio_pin_func(const int pin, char func)
{
    pin_state_t* pin_state = get_pin_state(pin);
    if (!pin_state)
    {
        errorf("Invalid pin for function configuration\n");
        return -1;
    }

    if (pin_state->func == func)
    {
        return 0;
    }

    switch (func)
    {
    case 'g': //GPIO
        gpio_set_function((uint)pin, GPIO_FUNC_SIO);
        break;
    case 'p': //PWM
        gpio_set_function((uint)pin, GPIO_FUNC_PWM);
        break;
    default:
        errorf("Unkown pin func '%c'\n", func);
        return -1;
    }
    pin_state->func = func;

    return 0;
}
