#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"

void vApplicationTickHook( void ){}

static void vGreedyTask( void *pvParameters )
{
    volatile uint8_t ucBuffer[ 600 ];
    for( ;; )
    {
        for( int i = 0; i < 600; i++ )
        {
            ucBuffer[ i ] = ( uint8_t ) i;
        }
        vTaskDelay( pdMS_TO_TICKS( 100 ) );
    }
}

int main( void )
{
    stdio_init_all();

    xTaskCreate( vGreedyTask, "Greedy", 128,
             NULL, 1, NULL );

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
