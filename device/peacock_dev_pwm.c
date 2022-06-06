#include <stdint.h>
#include <stdbool.h>
#include "hardware/pwm.h"
#include "hardware/timer.h"

#include "peacock_dev_pwm.h"
#include "peacock_dev_pins.h"
#include "peacock_dev_msg.h"

#define PWM_SLICE_COUNT 8
typedef struct
{
    char clk_div_mode;
    bool enabled;
    int div_int;
    int div_frc;
    int wrap;
    bool phase_correct;
} pwm_slice_t;

static pwm_slice_t pwm_slices[PWM_SLICE_COUNT] = { 0 };
static inline pwm_slice_t* _get_pwm_slice(int slice)
{
    if(slice < 0 || slice >= PWM_SLICE_COUNT)
    {
        errorf("PWM slice %i is out of range [0,%i]\n", slice, PWM_SLICE_COUNT-1);
        return NULL;
    }
    return &pwm_slices[slice];
}

int pck_pwm_slice_channel_num(const int pin, int* slice_num, int* channel_num)
{
    const pin_state_t* pin_state = get_pin_state(pin);
    if(!pin_state)
    {
        errorf("Fetching slice number requires a valid pin\n");
        return -1;
    }

    *slice_num = pwm_gpio_to_slice_num(pin);
    *channel_num = pwm_gpio_to_channel(pin);
    //errorf("returning slice=%i channel=%i\n", *slice_num, *channel_num);
    return 0;
}


int pck_pwm_config(const int slice_num, const char mode, const int div_int, const int div_frc, const int wrap, bool phase_correct)
{
    pwm_slice_t* slice = _get_pwm_slice(slice_num);
    if(!slice)
    {
        errorf("Could not get PWM slice %i to configure\n", slice_num);
        return -1;
    }

    if(slice->enabled)
    {
        errorf("Cannot configure PWM slice %i while enabled. Disable first!\n", slice_num);
        return -1;
    }

    //Only take this code path if we have a change to make
    if (div_int != slice->div_int || div_frc != slice->div_frc)
    {
        if (div_frc < 0)
        {
            errorf("Fractional clock division cannot be <0\n");
            return -1;
        }

        if (div_int < 0)
        {
            errorf("Integer clock division cannot be <0\n");
            return -1;
        }

        if (div_int == 0 && div_frc == 0)
        {
            errorf("Cannot divide clock by 0\n");
            return -1;
        }

        pwm_set_clkdiv_int_frac((uint)slice_num, (int)div_int, (int)div_frc);
        slice->div_int = div_int;
        slice->div_frc = div_frc;
        //errorf("PWM slice %i configured dividers int=%i, frac=%i\n", slice_num, div_int, div_frc);
    }

    //Only take this code path if we have a change to make
    if (wrap != slice->wrap)
    {
        if (wrap < 1 || wrap > 0xFFFF)
        {
            errorf("Wrap value %i is out of range [0,%i]\n", wrap, 0xFFFF);
            return -1;
        }

        pwm_set_wrap((uint)slice_num, (int)wrap);
        slice->wrap = wrap;
        //errorf("PWM slice %i configured wrap=%i\n", slice_num, wrap);
    }

    //Only take this code path if we have a change to make
    if (phase_correct != slice->phase_correct)
    {    
        pwm_set_phase_correct((uint)slice_num, phase_correct);
        slice->phase_correct = phase_correct;
        //errorf("PWM slice %i configured phase_correct=%i\n", slice_num, phase_correct);
    }

    if (mode != slice->clk_div_mode)
    {
        switch(mode)
        {
        case 'F': pwm_set_clkdiv_mode(slice_num,PWM_DIV_FREE_RUNNING); break;
        case 'h': pwm_set_clkdiv_mode(slice_num, PWM_DIV_B_HIGH);      break;
        case 'r': pwm_set_clkdiv_mode(slice_num, PWM_DIV_B_RISING);    break;
        case 'f': pwm_set_clkdiv_mode(slice_num, PWM_DIV_B_FALLING);   break;
        default:
            errorf("Unkown clock divider mode '%c'\n", mode);
            return -1;
        }
        slice->clk_div_mode = mode;
        //errorf("PWM slice %i configured clock_div=%c\n", slice_num, mode);
    }

    // errorf("Finished configuring pwm slice %i with mode=%c, div_int=%i, div_frac=%i, wrap=%i phase=%i\n", 
    //     slice_num,
    //     mode,
    //     div_int,
    //     div_frc,
    //     wrap,
    //     phase_correct
    // );
    return 0;
}

int pck_pwm_level(const int pin, const int level)
{
    const pin_state_t* pin_state = get_pin_state(pin);
    if (!pin_state)
    {
        errorf("Setting PWM pin level requires a valid pin\n");
        return -1;
    }

    if(pin_state->pwm.level == level)
    {
        return 0;
    }

    if (level < 1 || level > 0xFFFF)
    {
        errorf("Level value %i is out of range [0,%i]\n", level, 0xFFFF);
        return -1;
    }

    pwm_set_gpio_level(pin,level);
    //errorf("Set pwm level on pin %i to level %i\n", pin, level);
    return 0;
}

int pck_pwm_enable(const int slice_num, bool enabled)
{
    pwm_slice_t* slice = _get_pwm_slice(slice_num);
    if (!slice)
    {
        errorf("Could not get PWM slice %i to enable/disable\n", slice_num);
        return -1;
    }

    pwm_set_enabled(slice_num, enabled);
    slice->enabled = enabled;
    //errorf("Set slice %i to enabled=%i\n", slice_num, enabled);
    return 0;
}


int pck_pwm_get_counter(const int slice_num, pwm_count_t* count)
{
    pwm_slice_t* slice = _get_pwm_slice(slice_num);
    if (!slice)
    {
        errorf("Could not get PWM slice %i to return counter value\n", slice_num);
        return -1;
    }

    count->now_ts = time_us_32();
    count->count  = pwm_get_counter(slice_num);
    //errorf("Got pwm slice %i counter=%i at time=%i\n", slice_num, count->count, count->now_ts);
    return 0; 
}