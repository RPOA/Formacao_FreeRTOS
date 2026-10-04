#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"
#include "timers.h"

#define BUTTON_PIN 15 /* ao GND, pull-up interno */

#define vPrintStringAndNumber( s, n ) printf( "%s %lu\r\n", ( s ), ( unsigned long ) ( n ) )

static void vDeferredHandlingFunction( void *pvParameter1,
                                       uint32_t ulParameter2 )
{
    vPrintStringAndNumber( "Evento", ulParameter2 );
}

static void vButtonISR( uint gpio, uint32_t events )
{
    static uint32_t ulCount = 0;
    BaseType_t xWoken = pdFALSE;
    xTimerPendFunctionCallFromISR( vDeferredHandlingFunction,
                                   NULL, ulCount++, &xWoken );
    portYIELD_FROM_ISR( xWoken ); }

static void vPeriodicTask( void *pvParameters )
{
    for( ;; )
    {
        busy_wait_ms( 1000 ); /* a trabalhar */
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
} /* bloqueada */

int main( void )
{
    stdio_init_all();
    gpio_init( BUTTON_PIN );
    gpio_set_dir( BUTTON_PIN, GPIO_IN );
    gpio_pull_up( BUTTON_PIN );
    gpio_set_irq_enabled_with_callback( BUTTON_PIN,
        GPIO_IRQ_EDGE_FALL, true, vButtonISR );
        
    xTaskCreate( vPeriodicTask, "Periodic", 512, NULL,
                configTIMER_TASK_PRIORITY - 1, NULL );

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
