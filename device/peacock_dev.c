/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#include "pico/stdlib.h"
#include "tusb.h"

#include "../msg/msg.h"

int getchar_()
{
    return getchar();
}

__attribute__((format(printf, 1, 2)))
int send_str(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int result = vprintf(fmt, args);
    va_end(args);
    return result;
}

__attribute__ ((format (printf, 1, 2)))
void error(const char* fmt, ...)
{
    
    va_list args;
    va_start(args, fmt);
    const int err_len = vsnprintf(NULL,0, fmt, args);
    printf("E:%i:", err_len);
    vprintf(fmt,args);
    if(fmt[strlen(fmt) -1 ] != '\n')
    {
        putchar('\n');
    }    
    va_end(args);    
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
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, val);
    error("out");
    printf("S:2:i%i:i%i\n", pin, val);
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


//Wait until serial terminal attaches
void usb_wait()
{
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;    
    gpio_put(LED_PIN, 1);
    while (!tud_cdc_connected()) 
    {    
        sleep_ms(100);
    }
    gpio_put(LED_PIN, 0);
}

int main() {
    stdio_init_all();

    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    for(int k = 0; k < 5; k++)
    {
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_PIN, 0);
        sleep_ms(100);
    }
    
    usb_wait();
    
    error("Waiting for commands...");
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
