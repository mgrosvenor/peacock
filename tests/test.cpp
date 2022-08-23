#include <iostream>

#include <peacock/peacock.h>

#define TRY(x)          \
do{                     \
    int err = x;        \
    if(err < 0)         \
    {                   \
        return -1;      \
    }                   \
} while(0);             \

using namespace std;

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "usage: peacock_test DEV PIN [VALUE]\n");
        return -1;
    }
    cout << "Host starting.." << endl;
    const char* device = argv[1];
    const int pin = atoi(argv[2]);
    const int value = argc > 3 ? atoi(argv[3]) : -2; 

    TRY(pck_init(device, false));
    TRY(pck_gpio_pin_func(pin, 'g'));
    TRY(pck_gpio_pull(pin, false, false));
    if( value > -2 && value < 2){
        cout << "Setting pin (" << pin << ") value to " << value << endl;
        TRY(pck_gpio_out(pin, value));
    }
    else{
        const int input = pck_gpio_in(pin);
        cout << "Got pin (" << pin << ") value of " << input << endl;
    }

    pck_close();
    cout << "Host done" << endl;    
    return 0;
}
