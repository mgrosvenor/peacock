
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "peacock_msg.h"

static msg_funcs_t msgs = { 0 };
void init_msgs(const msg_funcs_t* msg_funcs)
{
    msgs = *msg_funcs;
}

//Consume all characters until a newline is found
static inline void chomp()
{
    for (char c = msgs.getchar(); c != '\n'; c = msgs.getchar())
    {
    }
}

bool param_b(const msg_t* msg, int idx)
{
    if(idx < 0 || idx >= msg->pcount)
    {
        msgs.errorf("Parameter index %i is out of range [0,%i]\n", idx, (int)msg->pcount -1);
        return -1;
    }

    const param_t* param = &msg->params[idx];
    if(param->type != 'b')
    {
        msgs.errorf("Parameter type is not 'b' for bool. Found '%c'\n", param->type);
        return -1;
    }

    return param->b;
}

char param_c(const msg_t* msg, int idx)
{
    if (idx < 0 || idx >= msg->pcount)
    {
        msgs.errorf("Parameter index %i is out of range [0,%i]\n", idx, (int)msg->pcount - 1);
        return -1;
    }

    const param_t* param = &msg->params[idx];
    if (param->type != 'c')
    {
        msgs.errorf("Parameter type is not 'c' for char. Found '%c'\n", param->type);
        return -1;
    }

    return param->c;
}

int param_i(const msg_t* msg, int idx)
{
    if (idx < 0 || idx >= msg->pcount)
    {
        msgs.errorf("Parameter index %i is out of range [0,%i]\n", idx, (int)msg->pcount - 1);
        return -1;
    }

    const param_t* param = &msg->params[idx];
    if (param->type != 'i')
    {
        msgs.errorf("Parameter type is not 'i' for int. Found '%c'\n", param->type);
        return -1;
    }

    return param->i;
}

char* param_s(const msg_t* msg, int idx)
{
    if (idx < 0 || idx >= msg->pcount)
    {
        msgs.errorf("Parameter index %i is out of range [0,%i]\n", idx, (int)msg->pcount - 1);
        return NULL;
    }

    const param_t* param = &msg->params[idx];
    if (param->type != 's')
    {
        msgs.errorf("Parameter type is not 's' for string. Found '%c'\n", param->type);
        return NULL;
    }

    return param->s;
}


static int get_bool(bool* out)
{
    int c = msgs.getchar();
    switch (c)
    {
        case '0': *out = false; break;
        case '1': *out = true;  break;
        default:
            msgs.errorf("Cannot parse bool. Expected [0,1] but got \"%c\"", c);
    }    

    return msgs.getchar();
}


static int get_alnum(char* out)
{
    int c = msgs.getchar();
    if(!isalnum(c))
    {
        msgs.errorf("Cannot parse char. Expected [a-zA-Z0-9] but got \"%c\"", c);
    }

    *out = c;

    return msgs.getchar();
}



static int get_int(int32_t* out)
{
    char c = 0;
    int sign = 1;

    int32_t result = 0;

    //Extract the sign from the first character (if it's there)
    c = msgs.getchar();
    if (c == '-')
    {
        sign = -1;
        c = msgs.getchar();
    }
    else if (c == '+')
    {
        c = msgs.getchar();
    }

    for (; c != ':' && c != '\n'; c = msgs.getchar())
    {
        if (!isdigit(c))
        {
            msgs.errorf("Cannot parse parameter. Expected a digit but got \"%c\"", c);
            chomp();
            return -1;
        }

        result = result * 10 + (c - '0');
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
        msgs.errorf("Could not get string length from parameter");
        return -1;
    }

    if (c != ':')
    {
        msgs.errorf("Expected a seperator ':' but not found [2]");
        chomp();
        return -1;
    }

    char* str = calloc(1, slen + 1);

    for (int i = 0; i < slen; i++)
    {
        str[i] = msgs.getchar();       
    }

    *out = str;

    return msgs.getchar();
}

int get_msg(msg_t* const msg)
{
    const int n1  = msgs.getchar();
    msg->name[0] = n1;
    if (!isalnum((unsigned)msg->name[0]))
    {
        msgs.errorf("Cannot parse message name[0] \"0x%02X\"", msg->name[0]);
        chomp();
        return -1;
    }

    const int n2 = msgs.getchar();
    msg->name[1] = n2;
    if (!isalnum((unsigned)msg->name[1]))
    {
        msgs.errorf("Cannot parse message name[1] \"0x%02X\"", msg->name[1]);
        chomp();
        return -1;
    }

    const int sep = msgs.getchar(); //Skip ':' seperator
    if (sep != ':')
    {
        msgs.errorf("Expected a seperator ':' but not found [0]");
        chomp();
        return -1;
    }

    const int err = get_int(&msg->pcount);
    if (err < 0)
    {
        msgs.errorf("Cannot parse parameter count");
        chomp();
        return -1;
    }    

    if (msg->pcount < 0)
    {
        msgs.errorf("Cannot have a negative parameter count");
        chomp();
        return -1;
    }

    if (msg->pcount > MAX_PARAM)
    {
        msgs.errorf("Too many parameters. Got %i but maximum is %i", (int)msg->pcount, MAX_PARAM);
        chomp();
        return -1;
    }

    int c = err; //Grab the last returned character
    for (int i = 0; i < msg->pcount; i++)
    {
        if (c == '\n' && i < msg->pcount - 1)
        {
            msgs.errorf("Expected %i parameters but ran out at %i", (int)msg->pcount, i + 1);
            chomp();
            return -1;
        }

        if (c != ':')
        {
            msgs.errorf("Expected a seperator ':' but not found [1.%i]", i);
            chomp();
            return -1;
        }

        c = msgs.getchar();

        switch (c)
        {
        case 'b':
            msg->params[i].type = 'b';
            c = get_bool(&msg->params[i].b);
            if (c < 0)
            {
                msgs.errorf("Could not get bool from parameter %i", i);
                return -1;
            }
            break;
        case 'c':
            msg->params[i].type = 'c';
            c = get_alnum(&msg->params[i].c);
            if (c < 0)
            {
                msgs.errorf("Could not get char from parameter %i", i);
                return -1;
            }
            break;
        case 'i': 
            msg->params[i].type = 'i';
            c = get_int(&msg->params[i].i);
            if(c < 0)
            {
                msgs.errorf("Could not get integer from parameter %i", i);
                return -1;
            }
            break;

        case 's':
            msg->params[i].type = 's';                
            c = get_str(&msg->params->s);
            if(c < 0)
            {
                msgs.errorf("Could not get string from parameter %i", i);
                return -1;
            }                                     
            break;

        default:
            msgs.errorf("Unexpected type \"%c\" for parameter %i", c, i);
            chomp();
            return -1; 
        }
    }

    if (c == '\r')
    {
        c = msgs.getchar();
    }

    if (c != '\n')
    {
        msgs.errorf("Expected %i parameters but got too many (c=0x%02X)", (int)msg->pcount, c);
        chomp();
        return -1;
    }

    return 0;
}


int send_msg(const msg_t* const msg)
{    
    int result = msgs.sendf("%c%c:%i", msg->name[0], msg->name[1], (int)msg->pcount);
    for(int i = 0; i < msg->pcount; i++){
        switch(msg->params[i].type)
        {
        case 'c': //Char
            result += msgs.sendf(":c%c", msg->params[i].c);
            break;
        case 'b': //Bool 
            result += msgs.sendf(":b%c", msg->params[i].b ? '1' : '0');
            break;
        case 'i': //Integer (signed)
            result += msgs.sendf(":i%i", (int)msg->params[i].i);
            break;
        case 's':  //String (with integer length)
            result += msgs.sendf(":s%i:%s", (int)strlen(msg->params[i].s), msg->params[i].s);
            break;
        default:
            //Can't send an error here because we'll get into a broken state. So just put in a dummy.
            result += msgs.sendf(":x%04x", (int)msg->params[i].i);
            
            //Can't put this error message out on the device because it will mess with inflight message
            if(!msgs.isdev) msgs.errorf("Unexpected type '%c' for parameter %i. Sending 'x' instead.", msg->params[i].type, i);
            break;            
        }        
    }
    result += msgs.sendf("\n");

    return result;
}

int is_msg_success(const msg_t* const msg, const char n0, const char n1, const int pcount)
{
    if(msg->name[0] != n0)
    {
        msgs.errorf("Wrong message name0 type. Expected '%c' but got '%c'!\n", n0, msg->name[0]);
        return -1;        
    }

    if(msg->name[0] != n0)
    {
        msgs.errorf("Wrong message name1 type. Expected '%c' but got '%c'!\n", n1, msg->name[1]);
        return -1;        
    }

    if(msg->pcount < 1)
    {
        msgs.errorf("Not enough parameters to check for success!\n");
        return -1;
    }

    if(msg->pcount - 1 != pcount)
    {
        msgs.errorf("Unexpected number of parameters returned. Expected %i but got %i!\n", pcount, (int)msg->pcount - 1);
        return -1;
    }

    const int s = msg->pcount - 1;
    if(msg->params[s].type != 'b')
    {
        msgs.errorf("Success feild must be a boolean!\n");
        return -1;
    }

    return msg->params[s].b;

}


void msg_free(msg_t* msg)
{
    for(int i = 0; i < msg->pcount; i++)
        if(msg->params[i].type == 's')
            if(msg->params[i].s)
                free(msg->params[i].s);
}