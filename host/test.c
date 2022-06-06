#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "peacock_host.h"

static volatile bool stop = false;

void catch_signal(int signum)
{
    (void)signum;
    stop = true;
}


void flash(int sleepus)
{
    const int pin = 25;
    pck_gpio_pin_func(pin, PCK_GPIO_FUNC_GPIO);
    pck_gpio_pull(pin, false, false);
    while(!stop)
    {
        pck_gpio_out(pin, 0);
        usleep(sleepus);
        pck_gpio_out(pin, 1);
        usleep(sleepus);
    }
}

void pulse()
{
    const int pin = 25;
    pck_gpio_pin_func(pin, PCK_GPIO_FUNC_PWM);
    pck_get_errors();
    
    int slice = -1;
    int channel = -1;
    pck_pwm_slice_channel_num(pin, &slice, &channel);    
    pck_get_errors();
    printf("Got slice=%i, channel=%i\n", slice, channel);

    pck_pwm_enable(slice, false);
    pck_get_errors();
    const int wrap = 5000;
    pck_pwm_config(slice, PCK_PWM_MODE_FREE, 1, 0, wrap, false);
    pck_get_errors();
    int level = 1;
    pck_pwm_level(pin, level);
    pck_get_errors();
    
    pck_pwm_enable(slice, true);
    pck_get_errors();
    printf("Waiting for exit...\n");
    //pwm_count_t count = {0};
    bool up = true;
    while(!stop)
    {
        //pck_pwm_get_counter(slice,&count);
        //pck_get_errors();
        //printf("Count=%i, ts=%i\n", count.count, count.now_ts);
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
        usleep(50*1000);
    }
}


int main(int argc, char** argv)
{
    signal(SIGINT, catch_signal);
    if (argc < 2)
    {
        fprintf(stderr, "usage: serial DEV\n");
        return -1;
    }
    printf("Host starting\n");
    const char* device = argv[1];
    pck_host_init(device);   
    pck_get_errors();

    pulse();    
    //flash(100*1000);
    
    pck_host_close();
    printf("Host is exiting\n");
    return 0;
}
