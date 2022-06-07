#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>

#define PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS -1
#define PICO_STDIO_DEFAULT_CRLF 0
#include <pico/stdio/driver.h>
#include <pico/stdio_usb.h>
#include <pico/stdlib.h>
#include <tusb.h>
#include <hardware/pwm.h>

#include <peacock_msg/peacock_msg.h>
#include <peacock_dev_msg.h>
#include <peacock_dev_pins.h>
#include <peacock_dev_gpio.h>
#include <peacock_dev_pwm.h>

static inline msg_t run_util_cmd(const msg_t* const msg, const char n0, const char n1)
{
    msg_t result = INIT_RMSG(n0, n1, 0);
    SET_MSG_SUCCESS(&result, false);
    switch (n1)
    {
    case 's': //Sleep (micros)
    {
        const int sleep_us = param_i(msg, 0);
        busy_wait_us_32(sleep_us);
        SET_MSG_SUCCESS(&result, true);
        return result;
    }    

    default:
        errorf("Unknown util command '%c'\n", n1);
        return result;
    }
}

static inline msg_t run_pwm_cmd(const msg_t* const msg, const char n0, const char n1)
{
    switch (n1)
    {
    case 's': return pck_pwm_slice_channel_num(msg, n0, n1);
    case 'c': return pck_pwm_config(msg, n0, n1);
    case 'l': return pck_pwm_level(msg, n0, n1);
    case 'e': return pck_pwm_enable(msg, n0, n1);
    case 'C': return pck_pwm_get_counter(msg, n0, n1);
    default:
        errorf("Unknown PWM command \"%c\"", n1);
        msg_t result = INIT_RMSG(n0, n1, 0);
        SET_MSG_SUCCESS(&result, false);
        return result;
    }
}

static inline msg_t run_gpio_cmd(const msg_t* const msg, const char n0, const char n1)
{
    switch (n1)
    {
    case 'o': return pck_gpio_out(msg, n0, n1); 
    case 'i': return pck_gpio_in(msg, n0, n1);
    case 'p': return pck_gpio_pull(msg, n0, n1);
    case 'f': return pck_gpio_pin_func(msg, n0, n1);
    default:
        errorf("Unknown GPIO command \"%c\"", n1);
        msg_t result = INIT_RMSG(n0, n1, 0);
        SET_MSG_SUCCESS(&result, false);
        return result;
    }
}


static inline msg_t run_cmd(const msg_t* msg)
{
    const char n0 = msg->name[0];
    const char n1 = msg->name[1];
    switch (n0)
    {
    case 'G': return run_gpio_cmd(msg, n0, n1);   
    case 'P': return run_pwm_cmd(msg, n0, n1);
    case 'U': return run_util_cmd(msg, n0, n1);
    default:
        errorf("Unknown command \"%c\"", n0);
        msg_t result = INIT_RMSG(n0, n1, 0);
        SET_MSG_SUCCESS(&result, false);
        return result;
    }
}


int main() {
    init_dev_msgs();
    stdio_init_all();
    stdio_set_translate_crlf(&stdio_usb, false);    
    pck_gpio_init_all();
    pck_gpio_pull_disable_all();

    for(int i = 0; ; i++)
    {        
        msg_t msg = {0};
        if(get_msg(&msg))
        {
            errorf("Failed to run command num %i", i);
            continue;
        }

        msg = run_cmd(&msg);
        send_msg(&msg);    
    }
    return 0;
}
