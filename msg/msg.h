#ifndef MSG
#define MSG

#include <stdint.h>


//ASCII encoded messages in the form of 
//M:P[:Tpppp..][:Tpppp...][...]\n
//Where 
// - N is the message name as a single ASCII alpha num [a-zA-Z0-9]
// - P is the number of parameters ASCII [0-9]
// - T is the paramter type 
//   - i for integer
//   - sXXX:... for string - String of length XXX (ASCII encoded digits)
// - pppp is the ASCII encoded message paramter

typedef struct {
    char type;
    union {
        int32_t i;
        char* s;
    };
} param_t;

#define MAX_PARAM 16
typedef struct {
    char name;
    int32_t pcount;
    param_t params[MAX_PARAM];
} msg_t;

int get_msg(msg_t* const msg);
void wait_init();
int send_msg(const msg_t* const msg);



#endif // MSG
