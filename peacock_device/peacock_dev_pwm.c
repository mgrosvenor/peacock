#include <stdint.h>
#include <stdbool.h>

#include <hardware/pwm.h>
#include <hardware/timer.h>

#include <peacock_dev_pwm.h>
#include <peacock_dev_pins.h>
#include <peacock_dev_msg.h>
#include <peacock_dev_err.h>

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


msg_t pck_pwm_slice_channel_num(const msg_t* const msg, const char n0, const char n1)
{   
    msg_t result = INIT_RMSG( n0, n1, 3);    
    SET_MSG_SUCCESS(&result,false);

    const int pin = param_i(msg, 0);
    const pin_state_t* pin_state = get_pin_state(pin);
    if(!pin_state)
    {
        errorf("Fetching slice number requires a valid pin\n");        
        return result;
    }

    const int slice_num = pwm_gpio_to_slice_num(pin);
    const int channel_num = pwm_gpio_to_channel(pin);

    SET_MSG_PARAM_I(&result,0,pin);
    SET_MSG_PARAM_I(&result,1,slice_num);
    SET_MSG_PARAM_I(&result,2,channel_num);
    SET_MSG_SUCCESS(&result,true);

    return result;
}


msg_t pck_pwm_config(const msg_t* const msg, const char n0, const char n1)
{

    msg_t result = INIT_RMSG(n0, n1, 0);
    SET_MSG_SUCCESS(&result,false);

    const int slice_num = param_i(msg, 0); 
    const char mode     = param_c(msg, 1);
    const int div_int   = param_i(msg, 2);
    const int div_frc   = param_i(msg, 3);
    const int wrap      = param_i(msg, 4);
    bool phase_correct  = param_b(msg, 5);
        
    pwm_slice_t* slice = _get_pwm_slice(slice_num);
    if(!slice)
    {
        errorf("Could not get PWM slice %i to configure\n", slice_num);
        SET_MSG_SUCCESS(&result,false);
        return result;
    }

    if(slice->enabled)
    {
        errorf("Cannot configure PWM slice %i while enabled. Disable first!\n", slice_num);
        return result;
    }

    //Only take this code path if we have a change to make
    if (div_int != slice->div_int || div_frc != slice->div_frc)
    {
        if (div_frc < 0)
        {
            errorf("Fractional clock division cannot be <0\n");
            return result;
        }

        if (div_int < 0)
        {
            errorf("Integer clock division cannot be <0\n");
            return result;
        }

        if (div_int == 0 && div_frc == 0)
        {
            errorf("Cannot divide clock by 0\n");
            return result;
        }

        pwm_set_clkdiv_int_frac((uint)slice_num, (int)div_int, (int)div_frc);
        slice->div_int = div_int;
        slice->div_frc = div_frc;
    }

    //Only take this code path if we have a change to make
    if (wrap != slice->wrap)
    {
        if (wrap < 1 || wrap > 0xFFFF)
        {
            errorf("Wrap value %i is out of range [0,%i]\n", wrap, 0xFFFF);
            return result;
        }

        pwm_set_wrap((uint)slice_num, (int)wrap);
        slice->wrap = wrap;
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
            return result;
        }
        slice->clk_div_mode = mode;
    }

    SET_MSG_SUCCESS(&result,true);
    return result;
}

msg_t pck_pwm_level(const msg_t* const msg, const char n0, const char n1)
{
    msg_t result = INIT_RMSG(n0, n1, 0);
    SET_MSG_SUCCESS(&result,false);

    const int pin = param_i(msg, 0);   
    const int level = param_i(msg, 1);

    const pin_state_t* pin_state = get_pin_state(pin);
    if (!pin_state)
    {
        errorf("Setting PWM pin level requires a valid pin\n");
        return result;
    }

    if(pin_state->pwm.level == level)
    {
        SET_MSG_SUCCESS(&result, true);
        return result;
    }

    if (level < 1 || level > 0xFFFF)
    {
        errorf("Level value %i is out of range [0,%i]\n", level, 0xFFFF);
        return result;
    }

    pwm_set_gpio_level(pin,level);
    
    SET_MSG_SUCCESS(&result,true);
    return result;
}

msg_t pck_pwm_enable(const msg_t* const msg, const char n0, const char n1)
{
    msg_t result = INIT_RMSG(n0, n1, 0);
    SET_MSG_SUCCESS(&result, false);
    
    const int slice_num = param_i(msg, 0);            
    const bool enabled  = param_b(msg, 1);
         
    pwm_slice_t* slice = _get_pwm_slice(slice_num);
    if (!slice)
    {
        errorf("Could not get PWM slice %i to enable/disable\n", slice_num);
        return result;
    }

    pwm_set_enabled(slice_num, enabled);
    slice->enabled = enabled;
    
    SET_MSG_SUCCESS(&result,true);
    return result;
}


msg_t pck_pwm_get_counter(const msg_t* const msg, const char n0, const char n1)
{
    msg_t result = INIT_RMSG(n0, n1, 4);
    SET_MSG_SUCCESS(&result, false);

    const int slice_num = param_i(msg, 0);
    pwm_slice_t* slice = _get_pwm_slice(slice_num);
    if (!slice)
    {
        errorf("Could not get PWM slice %i to return counter value\n", slice_num);
        return result;
    }

    const uint64_t start = time_us_64();
    const int count = pwm_get_counter(slice_num);
    //const uint64_t stop  = time_us_64();
    
    //Assume that the pwm_counter is fetched in the middle of the two timestamps 
    const uint64_t now_ts = start; //(start + stop) / 2;

    const int now_ts_lo = (uint32_t)( now_ts & 0x00000000FFFFFFFFULL       );
    const int now_ts_hi = (uint32_t)((now_ts & 0xFFFFFFFF00000000ULL) >> 32);    
    
    SET_MSG_PARAM_I(&result, 0, slice_num);
    SET_MSG_PARAM_I(&result, 1, count);
    SET_MSG_PARAM_I(&result, 2, now_ts_lo);
    SET_MSG_PARAM_I(&result, 3, now_ts_hi);
    SET_MSG_SUCCESS(&result, true);

    return result; 
}