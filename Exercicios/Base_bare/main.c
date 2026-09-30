#include <stdio.h>
#include "pico/stdlib.h"

int main( void )
{
    gpio_init( PICO_DEFAULT_LED_PIN );
    gpio_set_dir( PICO_DEFAULT_LED_PIN, GPIO_OUT );
    for( ;; )
    {
        gpio_xor_mask( 1u << PICO_DEFAULT_LED_PIN );
        sleep_ms( 500 );
    }
}
