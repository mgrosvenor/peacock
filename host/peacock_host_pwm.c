#include "peacock_host_pwm.h"
#include "peacock_host_err.h"
#include "peacock_host_msg.h"
#include "peacock_host_pins.h"

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

    msg_t msg =
    {
        .name = {'P', 's'},
        .pcount = 1,
        .params =
        {
            INT(pin),
        }
    };

    send_msg(&msg);

    const int max_attempts = 100;
    for (int i = 0; get_response(&msg); i++)
    {
        if (i > max_attempts)
        {
            errorf("Made %i attempts to get response but found none\n", max_attempts);
            return -1;
        }
    }

    if (msg.name[0] != 'P' && msg.name[1] != 's')
    {
        errorf("Unexpected reply message type. Expected 'Ps' but got '%c%c'\n", msg.name[0], msg.name[1]);
        return -1;
    }

    if (msg.pcount != 3)
    {
        errorf("Unexpected parameter count. Expected 3 but got %i\n", msg.pcount);
        return -1;
    }

    const int rx_pin     = param_i(&msg, 0);
    const int rx_slice   = param_i(&msg, 1);
    const int rx_channel = param_i(&msg, 2);

    if (rx_pin != pin)
    {
        errorf("Wrong pin numer! Expected %i but got %i\n", pin, rx_pin);
        return -1;
    }

    if(!_slice_valid(rx_slice))
    {
        errorf("Invalid slice returned from device %i\n", rx_slice);
        return -1;
    }

    *slice_num   = rx_slice;
    *channel_num = rx_channel;

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

    msg_t msg = {
        .name = { 'P', 'c' },
            .pcount = 6,
            .params =
        {
            INT(slice_num),
            CHAR(mode),
            INT(div_int),
            INT(div_frc),
            INT(wrap),
            BOOL(phase_correct),
        }
    };

    send_msg(&msg);
    const int max_attempts = 100;
    for (int i = 0; get_response(&msg); i++)
    {
        if (i > max_attempts)
        {
            errorf("Made %i attempts to get response but found none\n", max_attempts);
            return -1;
        }
    }

    if (msg.name[0] != 'P' && msg.name[1] != 'c')
    {
        errorf("Unexpected reply message type. Expected 'Pc' but got '%c%c'\n", msg.name[0], msg.name[1]);
        return -1;
    }

    if (msg.pcount != 2)
    {
        errorf("Unexpected parameter count. Expected 3 but got %i\n", msg.pcount);
        return -1;
    }

    const int rx_slice = param_i(&msg, 0);
    const int result = param_i(&msg, 1);

    if (rx_slice != slice_num)
    {
        errorf("Wrong slice numer! Expected %i but got %i\n", slice_num, rx_slice);
        return -1;
    }

    if(result)
    {
        errorf("Failed setting config. Device side failure: %i\n", result);
    }
    
    return result;
}


int pck_pwm_level(const int pin, const int level)
{
    if (!pin_valid(pin))
    {
        errorf("Invalid pin supplied %i\n", pin);
        return -1;
    }

    msg_t msg = {
        .name = { 'P', 'l' },
            .pcount = 2,
            .params =
        {
            INT(pin),
            INT(level),
        }
    };

    send_msg(&msg);
    return 0;
}


int pck_pwm_enable(const int slice, bool enabled)
{
    if (!_slice_valid(slice))
    {
        errorf("Invalid pin supplied %i\n", slice);
        return -1;
    }

    msg_t msg = {
        .name = { 'P', 'e' },
            .pcount = 2,
            .params =
        {
            INT(slice),
            BOOL(enabled),
        }
    };

    send_msg(&msg);
    return 0;
}


int pck_pwm_get_counter(const int slice, pwm_count_t* count)
{
    if (!_slice_valid(slice))
    {
        errorf("Invalid slice supplied %i\n", slice);
        return -1;
    }

    msg_t msg =
    {
        .name = {'P', 'C'},
        .pcount = 1,
        .params =
        {
            INT(slice),
        }
    };

    send_msg(&msg);

    const int max_attempts = 100;
    for (int i = 0; get_response(&msg); i++)
    {
        if (i > max_attempts)
        {
            errorf("Made %i attempts to get response but found none\n", max_attempts);
            return -1;
        }
    }

    if (msg.name[0] != 'P' && msg.name[1] != 'C')
    {
        errorf("Unexpected reply message type. Expected 'PC' but got '%c%c'\n", msg.name[0], msg.name[1]);
        return -1;
    }

    if (msg.pcount != 3)
    {
        errorf("Unexpected parameter count. Expected 3 but got %i\n", msg.pcount);
        return -1;
    }

    const int rx_slice = param_i(&msg, 0);
    const int rx_count = param_i(&msg, 1);
    const int rx_ts    = param_i(&msg, 2);

    if (rx_slice != slice)
    {
        errorf("Wrong slice numer! Expected %i but got %i\n", slice, rx_slice);
        return -1;
    }

    count->count = rx_count;
    count->now_ts = rx_ts;

    return 0;
}
