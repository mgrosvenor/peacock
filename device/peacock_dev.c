/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>

#define PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS -1
#define PICO_STDIO_DEFAULT_CRLF 0
#include "pico/stdio/driver.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "hardware/pwm.h"

#include "../msg/msg.h"
#include "peacock_dev_msg.h"
#include "peacock_dev_pins.h"
#include "peacock_dev_gpio.h"
#include "peacock_dev_pwm.h"


static inline int run_util_cmd(const msg_t* msg)
{
    switch (msg->name[1])
    {
    case 's': //Sleep (micros)
    {
        const int sleep_us = param_i(msg, 0);
        busy_wait_us_32(sleep_us);
        return 0;
    }    

    default:
        errorf("Unknown util command \"%c\"", msg->name[1]);
        return -1;
    }
}

static inline int run_pwm_cmd(const msg_t* msg)
{
    switch (msg->name[1])
    {
    case 's': //Pin number to slice/channel number
    {
        const int pin_num = param_i(msg, 0);
        int slice_num = -1;
        int channel_num = -1;
        const int err = pck_pwm_slice_channel_num(pin_num, &slice_num, &channel_num);
        if(err < 0) return err;

        const msg_t msg =
        {
            .name = { 'P', 's'},
            .pcount = 3,
            .params = 
            {
                INT(pin_num),
                INT(slice_num),
                INT(channel_num)
            }
        };
        send_msg(&msg);
        return 0;
    }
    case 'c': //Configure slice
    {
        const int slice_num = param_i(msg, 0); 
        const int result = pck_pwm_config
        (
            slice_num, //Slice
            param_c(msg, 1), //Mode
            param_i(msg, 2), //Div int
            param_i(msg, 3), //Div frac
            param_i(msg, 4), //Wrap
            param_b(msg, 5)  //Phase correct
        );
        const msg_t msg =
        {
            .name = { 'P', 'c'},
            .pcount = 2,
            .params =
            {
                INT(slice_num),
                INT(result)
            }
        };
        send_msg(&msg);
        return result;
    }
    case 'l': //Set level (duty cycle = level / wrap)
        return pck_pwm_level
        (
            param_i(msg, 0), //Pin
            param_i(msg, 1)  //Level
        );

    case 'e': // Enable PWM output on slice
        return pck_pwm_enable
        (
            param_i(msg, 0), //Slice
            param_b(msg, 1)  //Enabled
        );

    case 'C': // Get the pwm counter value (and timestamp when we got it)
    {
        pwm_count_t cnt = {0};
        const int slice_num = param_i(msg, 0);
        const int err = pck_pwm_get_counter(slice_num, &cnt);
        if(err) return err; 

        const msg_t msg = 
        {
            .name = {'P', 'c'},
            .pcount = 3,
            .params = 
            {
                INT(slice_num),
                INT(cnt.count),
                INT(cnt.now_ts)
            }
        };

        send_msg(&msg);
        return 0;
    }
        
    default:
        errorf("Unknown PWM command \"%c\"", msg->name[1]);
        return -1;
    }
}

static inline int run_gpio_cmd(const msg_t* msg)
{
    switch (msg->name[1])
    {
    case 'o': 
        return pck_gpio_out
        (
            param_i(msg, 0), //Pin
            param_i(msg, 1)  //Val
        ); 
    case 'i': 
    {
        const int pin = param_i(msg, 0);
        const int val = pck_gpio_in(pin);
        if (val < 0) return val;

        const msg_t msg =
        {
            .name = { 'G', 'i'},
            .pcount = 2,
            .params =
            {
                INT(pin),
                BOOL(val)
            }
        };

        send_msg(&msg);
        return 0;
    }
    case 'p':  //Set up pull up/downs
        return pck_gpio_pull
        (
            param_i(msg, 0), //Pin
            param_b(msg, 1), //Up
            param_b(msg, 2)  //Down
        );
    case 'f':  //Set up pull up/downs
        return pck_gpio_pin_func
        (
            param_i(msg, 0), //Pin
            param_c(msg, 1)  //Func g=gpio, p=pwm
        );
    default:
        errorf("Unknown GPIO command \"%c\"", msg->name[1]);
        return -1;
    }
}


static inline int run_cmd(const msg_t* msg)
{
    switch (msg->name[0])
    {
    case 'G': return run_gpio_cmd(msg);   
    case 'P': return run_pwm_cmd(msg);
    case 'U': return run_util_cmd(msg);
    default:
        errorf("Unknown command \"%c\"", msg->name[0]);
        return -1;
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
            errorf("Failed to get command %i", i);
            continue;
        }

        if(run_cmd(&msg))
        {
            errorf("Failed executing command %i", i);
            continue;
        }
    }
    return 0;
}
