#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <peacock/peacock.h>

static volatile bool stop = false;

static void catch_signal(int signum)
{
    (void)signum;
    stop = true;
}

static inline void flash(int sleepus)
{
    const int pin = 25;
    pck_gpio_pin_func(pin, PCK_GPIO_FUNC_GPIO);
    pck_gpio_pull(pin, false, false);
    while(!stop)
    {
        pck_gpio_out(pin, 0);
        pck_util_sleep(sleepus);
        pck_gpio_out(pin, 1);
        pck_util_sleep(sleepus);
    }
}


void pulse()
{
    const int pin = 25;
    pck_gpio_pin_func(pin, PCK_GPIO_FUNC_PWM);
    
    int slice = -1;
    int channel = -1;
    pck_pwm_slice_channel_num(pin, &slice, &channel);            

    pck_pwm_enable(slice, false);
    const int wrap = 5000;
    pck_pwm_config(slice, PCK_PWM_MODE_FREE, 1, 0, wrap, false);

    int level = 1;
    pck_pwm_level(pin, level);
    pck_pwm_enable(slice, true);

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
        pck_pwm_level(pin, level);
        pck_util_sleep(50*1000);
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

    pck_init(device);       

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
