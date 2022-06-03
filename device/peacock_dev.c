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
#include "hardware/gpio.h"

#include "../msg/msg.h"

int getchar_()
{
    return getchar();
}

__attribute__((format(printf, 1, 2)))
int sendf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int result = vprintf(fmt, args);    
    va_end(args);
    fflush(stdout);
    return result;
}

__attribute__ ((format (printf, 1, 2)))
void error(const char* fmt, ...)
{    
    va_list args;
    va_start(args, fmt);
    const int err_len = vsnprintf(NULL,0, fmt, args);
    va_end(args);

    char* err = calloc(1, err_len + 1);
    va_start(args, fmt);   
    vsnprintf(err,err_len,fmt, args);
    va_end(args);

    msg_t msg = 
    {
        .name = 'E',
        .pcount = 1,
        .params = 
        {
            { .type = 's' , .s = err}
        }
    };

    send_msg(&msg);    
}

#define PIN_COUNT 29
typedef struct
{
    bool out_en;
    int out;
    bool pull_up;
    bool pull_dwn;
} pin_state_t;

pin_state_t pins[PIN_COUNT] = { 0 };

void init_all_iopins()
{
    gpio_init_mask(~0); //Set all pins to GPIO mode
    gpio_set_dir_in_masked(~0); //Set all pins to input (HiZ)
    for (int i = 0; i < PIN_COUNT; i++)
    {
        pins[i].out_en = false;
        pins[i].out = 0;
        pins[i].pull_up = false;
        pins[i].pull_dwn = false;
        gpio_disable_pulls(i);
    }
}

static inline int do_get(const msg_t* msg)
{
    //get_pin(pin);
    (void)msg;
    return -1;
}

static inline int do_set(const msg_t* msg)
{
    const int pin = msg->params[0].i;
    const int val = msg->params[1].i;
        
    pin_state_t* pin_state = &pins[pin];
    //Don't unnecessarily drive the pins
    if (pin_state->out == val)
    {
        return 0;
    }

    if (val == -1)
    {
        gpio_set_dir(pin, GPIO_IN);
        pin_state->out    = -1;
        pin_state->out_en = false;
        return 0;
    }
    else
    {
        if(!pin_state->out_en)
        {
            gpio_set_dir(pin, GPIO_OUT);
        }
        gpio_put(pin, val);
        pin_state->out    = val;
        pin_state->out_en = true;
    }
    return 0;
}

static inline int run_cmd(const msg_t* msg)
{
    switch (msg->name)
    {
    case 'S': return do_set(msg); break;
    case 'G': return do_get(msg); break;
    default:
        error("Unknown function call \"%i\"", msg->name);
        return -1;
    }
}


// const uint LED_PIN = PICO_DEFAULT_LED_PIN;
// void flash(int count, int sleep)
// {
//     for (int i = 0; i < count; i++)
//     {
//         gpio_put(LED_PIN, 1);
//         sleep_ms(sleep);
//         gpio_put(LED_PIN, 0);
//         sleep_ms(sleep);
//     }
// }

int main() {
    init_all_iopins();
    
    stdio_init_all();
    stdio_set_translate_crlf(&stdio_usb, false);    
    
    for(int i = 0; ; i++)
    {        
        msg_t msg = {0};
        if(get_msg(&msg))
        {
            error("Failed to get command %i", i);
            continue;
        }

        if(run_cmd(&msg))
        {
            error("Failed executing command %i", i);
            continue;
        }
    }
    return 0;
}
