#ifndef MSG
#define MSG

#include <stdint.h>
#include <stdbool.h>

//ASCII encoded messages in the form of 
//M:P[:Tpppp..][:Tpppp...][...]\n
//Where 
// - N is the message name as a single ASCII alpha num [a-zA-Z0-9]
// - P is the number of parameters ASCII [0-9]
// - T is the paramter type
//   - b for bool 
//   - i for integer
//   - sXXX:... for string - String of length XXX (ASCII encoded digits)
// - pppp is the ASCII encoded message paramter

typedef struct 
{
    char type;
    union 
    {
        bool b;
        char c;
        int32_t i;
        char* s;
    };
} param_t;

#define MAX_PARAM 16
typedef struct 
{
    char name[2];
    int32_t pcount;
    param_t params[MAX_PARAM];
} msg_t;

//Base functions required to send and receive
typedef struct 
{
    bool isdev; 
    //Receive the next character from the serial stream
    int (*getchar)(void);
    
    //Transmit a formatted string onto the serial stream
    __attribute__((format(printf, 1, 2)))
    int (*sendf)(const char* fmt, ...);

    //Output an error (either by message or locally)
    __attribute__((format(printf, 1, 2)))
    void (*errorf)(const char* fmt, ...);
} msg_funcs_t; 


void init_msgs(const msg_funcs_t* msg_funcs);
int get_msg(msg_t* const msg);
int send_msg(const msg_t* const msg);

bool  param_b(const msg_t* msg, int idx);
char  param_c(const msg_t* msg, int idx);
int   param_i(const msg_t* msg, int idx);
char* param_s(const msg_t* msg, int idx);

#define INT(I)   { .type = 'i', .i = I}
#define CHAR(C)  { .type = 'c', .c = C}
#define BOOL(B)  { .type = 'b', .b = B}
#define STR(S)   { .type = 's', .s = S}

#endif // MSG
