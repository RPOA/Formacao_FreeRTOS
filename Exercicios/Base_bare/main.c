#include <stdio.h>
#include "pico/stdlib.h"

#define LED_PIN 1

int main( void )
{
    gpio_init( LED_PIN );
    gpio_set_dir( LED_PIN, GPIO_OUT );
    for( ;; )
    {
        gpio_xor_mask( 1u << LED_PIN );
        sleep_ms( 500 );
    }
}
