#ifndef PEACOCK_DEV_PWM
#define PEACOCK_DEV_PWM

#include "../msg/msg.h"

msg_t pck_pwm_slice_channel_num(const msg_t* const msg, const char n0, const char n1);
msg_t pck_pwm_config(const msg_t* const msg, const char n0, const char n1);
msg_t pck_pwm_level(const msg_t* const msg, const char n0, const char n1);
msg_t pck_pwm_enable(const msg_t* const msg, const char n0, const char n1);
msg_t pck_pwm_get_counter(const msg_t* const msg, const char n0, const char n1);

#endif // PEACOCK_DEV_PWM
