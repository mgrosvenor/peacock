
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#define PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS -1
#define PICO_STDIO_DEFAULT_CRLF 0
#include "pico/stdio/driver.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"
#include "tusb.h"

//Wait until serial terminal attaches
void usb_wait()
{
    while (!tud_cdc_connected()) 
    {    
        sleep_ms(100);
    }
}

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
void flash(int count)
{
    for(int i = 0; i < count; i++)
    {
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_PIN, 0);
        sleep_ms(100);
    }
}


int main() {
    stdio_init_all();
    stdio_set_translate_crlf(&stdio_usb, false);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    //printf("Hello world\n");
    int state = 0;             
    while(1){
        int c = getchar();
        printf("%c",c);
        gpio_put(LED_PIN, state);
        state = !state;
    }
    
    return 0;
}
