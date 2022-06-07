#include "hardware/gpio.h"

#include "peacock_dev_gpio.h" 
#include "peacock_dev_msg.h"


static inline void _gpio_init(const int pin, pin_state_t* pin_state)
{
    pin_state->gpio.out_en = false;
    pin_state->gpio.out = 0;
    pin_state->func = 'g';
    
    gpio_set_function((uint)pin, GPIO_FUNC_SIO);
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

//Set output on GPIO pin
msg_t pck_gpio_out(const msg_t* const msg, const char n0, const char n1)
{
    msg_t result = INIT_RMSG(n0, n1, 0);
    SET_MSG_SUCCESS(&result, false);

    const int pin =  param_i(msg, 0);
    const int val =  param_i(msg, 1);
    pin_state_t* pin_state = get_pin_state(pin);
    if(!pin_state)
    {
        errorf("Invalid pin for output");
        return result;
    }

    if( pin_state->func != 'g')
    {
        errorf("Pin is not configured for GPIO\n");
        return result;
    }

    //Don't unnecessarily drive the pins
    if (pin_state->gpio.out == val)
    {
        SET_MSG_SUCCESS(&result,true);
        return result;
    }

    if(val < -1 || val > 1)
    {
        errorf("Value value out of range [-1,0,1]. Got %i\n", val);
        return result;
    }

    if (val == -1)
    {
        gpio_set_dir((uint)pin, GPIO_IN);
        pin_state->gpio.out    = -1;
        pin_state->gpio.out_en = false;
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

    SET_MSG_SUCCESS(&result,true);
    return result;
}


//Fetch input from a given GPIO pin
msg_t pck_gpio_in(const msg_t* const msg, const char n0, const char n1)
{
    msg_t result = INIT_RMSG(n0, n1, 2);
    SET_MSG_SUCCESS(&result, false);

    const int pin =  param_i(msg, 0);
    pin_state_t* pin_state = get_pin_state(pin);
    if(!pin_state)
    {
        errorf("GPIO pin input not get pin state\n");
        return result;
    }

    if (pin_state->func != 'g')
    {
        errorf("Pin is not configured for GPIO\n");
        return result;
    }

    if (pin_state->gpio.out_en)
    {
        gpio_set_dir((uint)pin, GPIO_IN);
        pin_state->gpio.out_en = false;
    }

    const bool val = gpio_get((uint)pin);

    SET_MSG_PARAM_I(&result, 0, pin);
    SET_MSG_PARAM_B(&result, 1, val);
    SET_MSG_SUCCESS(&result,true);
    
    return result;
}


msg_t pck_gpio_pull(const msg_t* const msg, const char n0, const char n1)
{
    msg_t result = INIT_RMSG(n0, n1, 0);
    SET_MSG_SUCCESS(&result, false);

    int pin  = param_i(msg, 0);
    bool up  = param_b(msg, 1);
    bool dwn = param_b(msg, 2); 
            
    pin_state_t* pin_state = get_pin_state(pin);
    if (!pin_state)
    {
        errorf("Invalid pin for pull up/down");
        return result;
    }

    pin_state->pull.dwn = dwn;
    pin_state->pull.up = up;
    gpio_set_pulls(pin, up, dwn);

    SET_MSG_SUCCESS(&result, true);
    return result;
}

msg_t pck_gpio_pin_func(const msg_t* const msg, const char n0, const char n1)
{
    msg_t result = INIT_RMSG(n0, n1, 0);
    SET_MSG_SUCCESS(&result, false);

    const int pin = param_i(msg, 0);             
    const char func = param_c(msg, 1);

    pin_state_t* pin_state = get_pin_state(pin);
    if (!pin_state)
    {
        errorf("Invalid pin for function configuration\n");
        return result;
    }

    if (pin_state->func == func)
    {
        SET_MSG_SUCCESS(&result, true);
        return result;
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
        return result;
    }
    pin_state->func = func;

    SET_MSG_SUCCESS(&result, true);
    return result;
}
