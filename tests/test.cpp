#include <iostream>

#include <peacock/peacock.h>

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

    pck_init(device);       
    pck_gpio_pin_func(pin, 'g');
    pck_gpio_pull(pin, false, false);
    if( value > -2 && value < 2){
        cout << "Setting pin (" << pin << ") value to " << value << endl;
        pck_gpio_out(pin, value);
    }
    else{
        const int input = pck_gpio_in(pin);
        cout << "Got pin (" << pin << ") value of " << input << endl;
    }

    pck_close();
    cout << "Host done" << endl;    
    return 0;
}
