#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"
#include "queue.h"
#include "timers.h"

#define vPrintStringAndNumber( s, n ) printf( "%s %lu\r\n", ( s ), ( unsigned long ) ( n ) )
void vApplicationTickHook( void ){}

static TimerHandle_t xOneShotTimer, xAutoReloadTimer;

static void prvTimerCallback( TimerHandle_t xTimer )
{
    TickType_t xTimeNow = xTaskGetTickCount();
    uint32_t ulCount;
    ulCount = ( uint32_t )
              pvTimerGetTimerID( xTimer );
    ulCount++;
    vTimerSetTimerID( xTimer, ( void * ) ulCount );
    if( xTimer == xOneShotTimer )
        vPrintStringAndNumber( "One-shot", xTimeNow );
    else
    {
        vPrintStringAndNumber( "Auto-reload",
                               xTimeNow );
        if( ulCount == 5 )
            xTimerStop( xTimer, 0 );
    }
}

int main( void )
{
    stdio_init_all();
    xOneShotTimer = xTimerCreate( "One-shot", pdMS_TO_TICKS( 3333 ),
                                  pdFALSE, ( void * ) 0, prvTimerCallback );
    xAutoReloadTimer = xTimerCreate( "Auto-reload", pdMS_TO_TICKS( 500 ),
                                     pdTRUE, ( void * ) 0, prvTimerCallback );
    xTimerStart( xOneShotTimer, 0 );
    xTimerStart( xAutoReloadTimer, 0 );

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
