
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "msg.h"

//These functions need to be defined by the user
extern void error(const char* fmt, ...);
extern int getchar_();
extern int send_str(const char* fmt, ...);

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

    return 0;
}


static int get_str(char** out)
{
    *out = NULL;    
    int32_t slen = 0;
    if (get_int(&slen))
    {
        error("Could not get string length from parameter");
        return -1;
    }

    char* str = calloc(1, slen + 1);

    for (int i = 0; i < slen; i++)
    {
        str[i] = getchar_();       
    }

    char c = getchar_();
    if (c != ':')
    {
        error("Expected seperator ':'");
        chomp();
        return -1;
    }

    *out = str;

    return 0;
}

int get_msg(msg_t* const msg)
{
    msg->name = getchar_();
    if (!isalnum((unsigned)msg->name))
    {
        error("Cannot parse message name \"%c\"", msg->name);
        chomp();
        return -1;
    }

    char sep = getchar_(); //Skip ':' seperator
    if (sep != ':')
    {
        error("Expected seperator ':'");
        chomp();
        return -1;
    }

    if (get_int(&msg->pcount))
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

    char c = 0;
    for (int i = 0; i < msg->pcount; i++)
    {
        c = getchar_();
        if (c == '\n' && i < msg->pcount - 1)
        {
            error("Expected %i parameters but ran out at %i", msg->pcount, i + 1);
            chomp();
            return -1;
        }

        switch (c)
        {
        case 'i': 
            msg->params[i].type = 'i';
            if(get_int(&msg->params[i].i))
            {
                error("Could not get integer from parameter %i", i);
                return -1;
            }
            break;

        case 's':
            msg->params[i].type = 's';                
            if(get_str(&msg->params->s))
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

    if (c != '\n')
    {
        error("Expected %i parameters but got too many", msg->pcount);
        chomp();
        return -1;
    }

    return 0;
}


int send_msg(const msg_t* const msg)
{    
    int result = send_str("%c:%i", msg->name, msg->pcount);
    for(int i = 0; i < msg->pcount; i++){
        switch(msg->params[i].type)
        {
            case 'i': 
                result += send_str("i:%i", msg->params->i);
                break;
            case 's': 
                result += send_str("s%i:%s", strlen(msg->params->s), msg->params->s);
                break;
            default:
                error("Unknown parameter type \"%c\"", msg->params[i]);
                return -1; 
        }        
    }
    result += send_str("\n");

    return result;
}


void msg_free(msg_t* msg)
{
    for(int i = 0; i < msg->pcount; i++)
        if(msg->params[i].type == 's')
            if(msg->params[i].s)
                free(msg->params[i].s);
}