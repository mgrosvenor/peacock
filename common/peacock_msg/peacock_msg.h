#ifndef PEACOCK_MSG
#define PEACOCK_MSG

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

    //Distinguish errors and debug 
    __attribute__((format(printf, 1, 2)))
    void (*debugf)(const char* fmt, ...);
} msg_funcs_t; 


void init_msgs(const msg_funcs_t* msg_funcs);
int get_msg(msg_t* const msg);
int send_msg(const msg_t* const msg);

bool  param_b(const msg_t* msg, int idx);
char  param_c(const msg_t* msg, int idx);
int   param_i(const msg_t* msg, int idx);
char* param_s(const msg_t* msg, int idx);

#define INT(I)   (param_t){ .type = 'i', .i = I}
#define CHAR(C)  (param_t){ .type = 'c', .c = C}
#define BOOL(B)  (param_t){ .type = 'b', .b = B}
#define STR(S)   (param_t){ .type = 's', .s = S}


//Initialise a request message
// NAME0/NAME1 are the group and function names 
// PCOUNT is the number parameters to be returned 
#define INIT_MSG(NAME0,NAME1,PCOUNT)\
(msg_t){                            \
    .name = { NAME0, NAME1},        \
    .pcount = PCOUNT,               \
}


//Initialise a return message 
// NAME0/NAME1 are the group and function names 
// PCOUNT is the number parameters to be returned 
// (in addition to success/failure parameter)
#define INIT_RMSG(NAME0,NAME1,PCOUNT)\
(msg_t){                            \
    .name = { NAME0, NAME1},        \
    .pcount = PCOUNT+1              \
}

#define SET_MSG_SUCCESS(msg, V)             \
    do{                                     \
        const int r = (msg)->pcount - 1;    \
        (msg)->params[r].type = 'b';        \
        (msg)->params[r].b = V;             \
    }                                       \
    while(0)

#define _SET_MSG_PARAM(msg, P, T, M, V)  \
    do{                                  \
        (msg)->params[P].type = T;       \
        (msg)->params[P].M = V;          \
    }                                    \
    while(0)

int is_msg_success(const msg_t* const msg, const char n0, const char n1, const int pcount);

#define SET_MSG_PARAM_B(msg, P, V) _SET_MSG_PARAM(msg, P, 'b', b, V)
#define SET_MSG_PARAM_C(msg, P, V) _SET_MSG_PARAM(msg, P, 'c', c, V)
#define SET_MSG_PARAM_I(msg, P, V) _SET_MSG_PARAM(msg, P, 'i', i, V)
#define SET_MSG_PARAM_S(msg, P, V) _SET_MSG_PARAM(msg, P, 's', s, V)


#endif // PEACOCK_MSG
