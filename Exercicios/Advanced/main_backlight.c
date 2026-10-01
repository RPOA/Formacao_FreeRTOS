#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"
#include "queue.h"
#include "timers.h"

#define BUTTON_PIN 15 /* ao GND, pull-up interno */

#define vPrintStringAndNumber( s, n ) printf( "%s %lu\r\n", ( s ), ( unsigned long ) ( n ) )
void vApplicationTickHook( void ){}

static TimerHandle_t xBacklightTimer; /* 5 s */

static void vKeyHitTask( void *pvParameters )
{
    const TickType_t xShortDelay = pdMS_TO_TICKS( 50 );
    for( ;; )
    {
        if( gpio_get( BUTTON_PIN ) == 0 ) /* premido */
        {
            gpio_put( PICO_DEFAULT_LED_PIN, 1 );
            xTimerReset( xBacklightTimer,
                         xShortDelay );
        }
        vTaskDelay( xShortDelay );
    }
}

static void prvBacklightTimerCallback( TimerHandle_t xTimer )
{
    gpio_put( PICO_DEFAULT_LED_PIN, 0 ); /* apaga */
    vPrintStringAndNumber( "Backlight OFF, tick",
                           xTaskGetTickCount() );
}

int main( void )
{
    stdio_init_all();
    gpio_init( BUTTON_PIN );
    gpio_set_dir( BUTTON_PIN, GPIO_IN );
    gpio_pull_up( BUTTON_PIN );

    xBacklightTimer = xTimerCreate( "Backlight", pdMS_TO_TICKS( 5000 ),
                                    pdFALSE, ( void * ) 0, prvBacklightTimerCallback );
    xTimerStart( xBacklightTimer, 0 );

    xTaskCreate( vKeyHitTask, "KeyHit", 256, NULL, 1, NULL );

    vTaskStartScheduler();
    
    for( ;; ); /* só chega aqui se faltar heap */
}

void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char *pcTaskName )
{
    ( void ) xTask;
    ( void ) pcTaskName; /* ver no debugger */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

void vApplicationMallocFailedHook( void )
{
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
