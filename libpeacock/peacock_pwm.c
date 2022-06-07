#include <libpeacock/peacock_pwm.h>
#include <libpeacock/peacock_err.h>
#include <libpeacock/peacock_msg.h>
#include <libpeacock/peacock_pins.h>

#define PWM_SLICE_COUNT 8

static inline int _slice_valid(const int slice)
{
    return slice > 0 && slice < PWM_SLICE_COUNT;
}

int pck_pwm_slice_channel_num(const int pin, int* slice_num, int* channel_num)
{
    if (!pin_valid(pin))
    {
        errorf("Invalid pin %i\n", pin);
        return -1;
    }

    const char n0 = 'P'; //PWM group
    const char n1 = 's'; //Slice/channel function

    msg_t msg = INIT_MSG(n0, n1, 1);
    SET_MSG_PARAM_I(&msg, 0, pin);
    send_msg(&msg);    

    if(pck_next_msg(&msg))
    {
        errorf("Could not get message PWM slice/channel response\n");
    }

    int success = is_msg_success(&msg, n0, n1, 3);
    if(success < 1)
    {
        errorf("PWM slice/channel fetch command failed!\n");
        return -1;
    }    

    const int msg_pin   = param_i(&msg, 0);
    const int msg_slice = param_i(&msg, 1);
    const int msg_chan  = param_i(&msg, 2);

    if(msg_pin != pin)
    {
        errorf("Got the wrong pin back. Expected %i, but got %i\n", pin, msg_pin);
        return -1;
    }

    *slice_num   = msg_slice;
    *channel_num = msg_chan;

    return 0;
}


int pck_pwm_config(const int slice_num, const char mode, const int div_int, const int div_frc, const int wrap, bool phase_correct)
{
    if(!_slice_valid(slice_num))
    {
        errorf("Invalid slice supplied %i\n", slice_num);
        return -1;
    }

    if(mode != PCK_PWM_MODE_FREE  && 
       mode != PCK_PWM_MODE_HIGH  &&
       mode != PCK_PWM_MODE_RISE  &&
       mode != PCK_PWM_MODE_FALL  )
    {
        errorf("Unexpected PWM mode type. Expected [F,h,r,f] but got %c\n", mode);
        return -1;
    }

    if( div_int == 0 && div_frc == 0)
    {
        errorf("Cannot set clock divider to 0\n");
        return -1;
    }

    if(div_int < 0 || div_frc < 0)
    {
        errorf("Cannot set clock divider to negative value\n");
        return -1;
    }

    if(wrap <= 0 || wrap > 0xFFFF)
    {
        errorf("Wrap value range is [1..%i], but got %i\n", 0xFFFF, wrap );
        return -1;
    }

    const char n0 = 'P'; //PWM group
    const char n1 = 'c'; //Configure PWM slice function

    msg_t msg = INIT_MSG(n0, n1, 6);
    SET_MSG_PARAM_I(&msg, 0, slice_num);
    SET_MSG_PARAM_C(&msg, 1, mode);
    SET_MSG_PARAM_I(&msg, 2, div_int);
    SET_MSG_PARAM_I(&msg, 3, div_frc);
    SET_MSG_PARAM_I(&msg, 4, wrap);
    SET_MSG_PARAM_B(&msg, 5, phase_correct);
    send_msg(&msg);
    
    return pck_success(n0, n1, 0);
}


int pck_pwm_level(const int pin, const int level)
{
    if (!pin_valid(pin))
    {
        errorf("Cannot set level with invalid pin %i\n", pin);
        return -1;
    }

    if(level <= 0 || level > 0xFFFF)
    {
        errorf("Level value range is [1..%i], but got %i\n", 0xFFFF, level );
        return -1;
    }

    const char n0 = 'P'; //PWM group
    const char n1 = 'l'; //Set level function

    msg_t msg = INIT_MSG(n0, n1, 2);
    SET_MSG_PARAM_I(&msg, 0, pin);
    SET_MSG_PARAM_I(&msg, 1, level);
    send_msg(&msg);
    return pck_success(n0, n1, 0);
}


int pck_pwm_enable(const int slice, bool enabled)
{
    if (!_slice_valid(slice))
    {
        errorf("Invalid pin supplied %i\n", slice);
        return -1;
    }

    const char n0 = 'P'; //PWM group
    const char n1 = 'e'; //Enable PWM function

    msg_t msg = INIT_MSG(n0, n1, 2);
    SET_MSG_PARAM_I(&msg, 0, slice);
    SET_MSG_PARAM_B(&msg, 1, enabled);
    send_msg(&msg);
    return pck_success(n0, n1, 0);
}


int pck_pwm_get_counter(const int slice, pwm_count_t* count)
{
    if (!_slice_valid(slice))
    {
        errorf("Invalid slice supplied %i\n", slice);
        return -1;
    }

    const char n0 = 'P'; //PWM group
    const char n1 = 'C'; //get counter function

    msg_t msg = INIT_MSG(n0, n1, 1);
    SET_MSG_PARAM_I(&msg, 0, slice);
    send_msg(&msg);
    
    if(pck_next_msg(&msg))
    {
        errorf("Could not get message PWM counter fetch response\n");
    }

    int success = is_msg_success(&msg, n0, n1, 4);
    if(success < 1)
    {
        errorf("PWM counter fetch command failed!\n");
        return -1;
    }    

    const int msg_slice  = param_i(&msg, 0);
    const int msg_count  = param_i(&msg, 1);
    const int msg_ts_lo  = param_i(&msg, 2);
    const int msg_ts_hi  = param_i(&msg, 3);

    if(msg_slice != slice)
    {
        errorf("Got the wrong slice back. Expected %i, but got %i\n", slice, msg_slice);
        return -1;
    }

    count->count = msg_count;
    count->ts_lo = msg_ts_lo;
    count->ts_hi = msg_ts_hi;

    return 0;
}
