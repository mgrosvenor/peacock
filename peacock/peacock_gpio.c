#include <peacock/peacock_gpio.h>
#include <peacock/peacock_msg.h>
#include <peacock/peacock_pins.h>
#include <peacock/peacock_err.h>


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

    const char n0 = 'G'; //GPIO group
    const char n1 = 'o'; //Output function

    msg_t msg = INIT_MSG(n0, n1, 2);
    SET_MSG_PARAM_I(&msg, 0, pin);
    SET_MSG_PARAM_I(&msg, 1, val);
    return pck_do_msg_ts(&msg, n0, n1, 0);
}


//Fetch input from a given GPIO pin
int pck_gpio_in(const int pin)
{
    if (!pin_valid(pin))
    {
        errorf("Invalid pin %i\n", pin);
        return -1;
    }

    const char n0 = 'G'; //GPIO group
    const char n1 = 'i'; //input function

    msg_t msg = INIT_MSG(n0, n1, 1);
    SET_MSG_PARAM_I(&msg, 0, pin);
    
    const int res = pck_do_msg_ts(&msg,n0,n1,2);
    if (res < 0)
    {
        errorf("Could not get message GPIO input response\n");
        return -1;
    }

    const int msg_pin = param_i(&msg, 0);
    const bool msg_val = param_b(&msg, 1);

    if(msg_pin != pin)
    {
        errorf("Got the wrong pin back. Expected %i, but got %i\n", pin, msg_pin);
        return -1;
    }

    return msg_val;
}

//Set the pull ups/downs on the give pin
int pck_gpio_pull(int pin, bool up, bool dwn)
{
    if (!pin_valid(pin))
    {
        errorf("Invalid pin %i\n", pin);
        return -1;
    }

    const char n0 = 'G'; //GPIO group
    const char n1 = 'p'; //Pull up/down function

    msg_t msg = INIT_MSG(n0, n1, 3);
    SET_MSG_PARAM_I(&msg, 0, pin);
    SET_MSG_PARAM_B(&msg, 1, up);
    SET_MSG_PARAM_B(&msg, 2, dwn);
    return pck_do_msg_ts(&msg, n0, n1, 0);
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

    const char n0 = 'G'; //GPIO group
    const char n1 = 'f'; //Function mode

    msg_t msg = INIT_MSG(n0, n1, 2);
    SET_MSG_PARAM_I(&msg, 0, pin);
    SET_MSG_PARAM_C(&msg, 1, func);
    return pck_do_msg_ts(&msg, n0, n1, 0);
}
