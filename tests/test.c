#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <peacock/peacock.h>

#define TRY(x)          \
do{                     \
    int err = x;        \
    if(err < 0)         \
    {                   \
        return;         \
    }                   \
} while(0);             \

static volatile bool stop = false;

static void catch_signal(int signum)
{
    (void)signum;
    stop = true;
}

static inline void flash(int sleepus)
{
    const int pin = 25;    
    TRY(pck_gpio_pin_func(pin, PCK_GPIO_FUNC_GPIO));
    TRY(pck_gpio_pull(pin, false, false));
    while(!stop)
    {
        TRY(pck_gpio_out(pin, 0));
        TRY(pck_util_sleep(sleepus));
        TRY(pck_gpio_out(pin, 1));
        TRY(pck_util_sleep(sleepus));
    }
}


void pulse()
{
    const int pin = 25;
    TRY(pck_gpio_pin_func(pin, PCK_GPIO_FUNC_PWM));
    
    int slice = -1;
    int channel = -1;
    TRY(pck_pwm_slice_channel_num(pin, &slice, &channel));

    TRY(pck_pwm_enable(slice, false));
    const int wrap = 5000;
    TRY(pck_pwm_config(slice, PCK_PWM_MODE_FREE, 1, 0, wrap, false));

    int level = 1;
    TRY(pck_pwm_level(pin, level));
    TRY(pck_pwm_enable(slice, true));

    bool up = true;
    while(!stop)
    {
        if(up)
        {
            if (level < wrap)
            {
                level *= 2;
            }
            else
            {
                up = false;
                level = wrap;
            }
        }
        else
        {
            if (level > 2)
            {
                level /= 2;
            }
            else
            {
                up = true;
                level = 1;
            }
        }
        TRY(pck_pwm_level(pin, level));
        TRY(pck_util_sleep(50 * 1000));
    }
}


int main(int argc, char** argv)
{
    signal(SIGINT, catch_signal);
    if (argc < 3)
    {
        fprintf(stderr, "usage: peacock_test DEV MODE\n");
        return -1;
    }
    printf("Host starting\n");
    const char* device = argv[1];
    const int mode = atoi(argv[2]);

    int err = pck_init(device);       
    if(err)
    {
        return -1;
    }

    if(mode == 0)
    {
        pulse();    
    }
    else
    {
        flash(100*1000);
    }
    
    pck_close();
    printf("Host is exiting\n");
    return 0;
}
