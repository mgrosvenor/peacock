
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "msg.h"

//These functions need to be defined by the user
extern void error(const char* fmt, ...);
extern int getchar_();
extern int sendf(const char* fmt, ...);

//Consume all characters until a newline is found
static inline void chomp()
{
    for (char c = getchar_(); c != '\n'; c = getchar_())
    {
    }
}


static int get_int(int32_t* out)
{
    char c = 0;
    int mult = 1;
    int sign = 1;

    int32_t result = 0;

    //Extract the sign from the first character (if it's there)
    c = getchar_();
    if (c == '-')
    {
        sign = -1;
        c = getchar_();
    }
    else if (c == '+')
    {
        c = getchar_();
    }

    for (; c != ':' && c != '\n'; c = getchar_())
    {
        if (!isdigit(c))
        {
            error("Cannot parse parameter. Expected a digit but got \"%c\"", c);
            chomp();
            return -1;
        }

        result *= mult;
        result += (c - '0');
        mult *= 10;
    }

    result *= sign;
    *out = result;

    return c;
}


static int get_str(char** out)
{
    *out = NULL;    
    int32_t slen = 0;
    int c = get_int(&slen);
    if ( c < 0)
    {
        error("Could not get string length from parameter");
        return -1;
    }

    if (c != ':')
    {
        error("Expected a seperator ':' but not found [2]");
        chomp();
        return -1;
    }

    char* str = calloc(1, slen + 1);

    for (int i = 0; i < slen; i++)
    {
        str[i] = getchar_();       
    }

    *out = str;

    return getchar_();
}

// #define JUNK_BUFF 4096
// void wait_init()
// {
//     error("Waiting for init...\n");
//     char buff[JUNK_BUFF] = { 0 };
//     char* b = buff;
//     //Chomp away any junk until the start of a message
//     int c = -1; 
//     for(c = getchar_(); c != 'X'; c = getchar_())
//     {
//         int l = sprintf(b, "0x%0X[%c] ", c, isalnum(c) ? c : '.');
//         b += l;
//         if (b >= buff + JUNK_BUFF)
//         {
//             error("too much junk\n");
//             return;
//         }
//     }
//     error("chomp=%s", buff);

//     error("msg->name='''%c'''\n", c);
//     if (!isalnum((unsigned)c))
//     {
//         error("Cannot parse message name \"%c\"", c);
//         chomp();
//         return;
//     }

//     c = getchar_(); //Skip ':' seperator
//     if (c != ':')
//     {
//         error("Expected a seperator ':' but not found [0]");
//         chomp();
//         return;
//     }

//     c = getchar_(); //Skip ':' seperator
//     if (c != '0')
//     {
//         error("Expected no parameters but got '%c'", c);
//         chomp();
//         return;
//     }

//     c = getchar_();
//     if (c == '\r')
//     {
//         c = getchar_();
//     }

//     if (c != '\n')
//     {
//         error("Expected end of message but found '%c'\n");
//         chomp();
//         return;
//     }

// }



int get_msg(msg_t* const msg)
{
    int c = getchar_();
    msg->name = c;
    if (!isalnum((unsigned)msg->name))
    {
        error("Cannot parse message name \"%c\"", msg->name);
        chomp();
        return -1;
    }

    char sep = getchar_(); //Skip ':' seperator
    if (sep != ':')
    {
        error("Expected a seperator ':' but not found [0]");
        chomp();
        return -1;
    }

    c = get_int(&msg->pcount);
    if (c < 0)
    {
        error("Cannot parse parameter count");
        return -1;
    }    

    if (msg->pcount > MAX_PARAM)
    {
        error("Too many parameters. Got %i but maximum is %i", msg->pcount, MAX_PARAM);
        chomp();
        return -1;
    }

    for (int i = 0; i < msg->pcount; i++)
    {
        if (c == '\n' && i < msg->pcount - 1)
        {
            error("Expected %i parameters but ran out at %i", msg->pcount, i + 1);
            chomp();
            return -1;
        }

        if (c != ':')
        {
            error("Expected a seperator ':' but not found [1.%i]", i);
            chomp();
            return -1;
        }

        c = getchar_();

        switch (c)
        {
        case 'i': 
            msg->params[i].type = 'i';
            c = get_int(&msg->params[i].i);
            if(c < 0)
            {
                error("Could not get integer from parameter %i", i);
                return -1;
            }
            break;

        case 's':
            msg->params[i].type = 's';                
            c = get_str(&msg->params->s);
            if(c < 0)
            {
                error("Could not get string from parameter %i", i);
                return -1;
            }                                     
            break;

        default:
            error("Unexpected type \"%c\" for parameter %i", c, i);
            chomp();
            return -1; 
        }
    }

    if (c == '\r')
    {
        c = getchar_();
    }

    if (c != '\n')
    {
        error("Expected %i parameters but got too many (c=0x%02X)", msg->pcount, c, c);
        chomp();
        return -1;
    }

    return 0;
}


int send_msg(const msg_t* const msg)
{    
    int result = sendf("%c:%i", msg->name, msg->pcount);
    for(int i = 0; i < msg->pcount; i++){
        switch(msg->params[i].type)
        {
            case 'i': 
                result += sendf(":i%i", msg->params[i].i);
                break;
            case 's': 
                result += sendf(":s%i:%s", strlen(msg->params[i].s), msg->params[i].s);
                break;
            default:
                error("Unknown parameter type \"%c\"", msg->params[i]);
                return -1; 
        }        
    }
    result += sendf("\n");

    return result;
}


void msg_free(msg_t* msg)
{
    for(int i = 0; i < msg->pcount; i++)
        if(msg->params[i].type == 's')
            if(msg->params[i].s)
                free(msg->params[i].s);
}