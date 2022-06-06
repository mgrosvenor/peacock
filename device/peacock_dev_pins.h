#ifndef PEACOCK_DEV_PINS
#define PEACOCK_DEV_PINS

#include <stdint.h>
#include <stdbool.h>

//Generic state info for each pin

#define PIN_COUNT 29
typedef struct
{
    char func;
    union 
    {
        struct 
        {
            bool out_en;
            int out;
        } gpio;
        struct
        {
            int level;
        } pwm;
    };
    struct 
    {
        bool up;
        bool dwn;
    } pull;

} pin_state_t;

extern pin_state_t pins[PIN_COUNT];

pin_state_t* get_pin_state(const int pin);


#endif // PEACOCK_DEV_PINS
