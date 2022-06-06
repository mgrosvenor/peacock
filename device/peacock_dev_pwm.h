#ifndef PEACOCK_DEV_PWM
#define PEACOCK_DEV_PWM

#include <stdbool.h>

int pck_pwm_slice_channel_num(const int pin, int* slice_num, int* channel_num);
int pck_pwm_config(const int slice_num, const char mode, const int div_int, const int div_frc, const int wrap, bool phase_correct);
int pck_pwm_level(const int pin, const int level);
int pck_pwm_enable(const int slice, bool enabled);

typedef struct 
{
    int now_ts;
    int count;
} pwm_count_t;
int pck_pwm_get_counter(const int slice, pwm_count_t* count );


#endif // PEACOCK_DEV_PWM
