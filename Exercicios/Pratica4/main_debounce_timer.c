#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"
#include "timers.h"
#include "semphr.h"

#define BUTTON_PIN 15 /* ao GND, pull-up interno */

#define vPrintString( s ) printf( "%s", ( s ) )

static SemaphoreHandle_t xButtonSem;
static TimerHandle_t xDebounceTimer;
static volatile uint32_t ulIsrCount, ulButtonPressCounts;

static void vButtonCallback( uint gpio,
                             uint32_t events )
{
    BaseType_t xWoken = pdFALSE;
    ulIsrCount++;
    xTimerResetFromISR( xDebounceTimer, &xWoken );
    portYIELD_FROM_ISR( xWoken );
}

static void vDebounceCallback( TimerHandle_t xTimer )
{
    if( gpio_get( BUTTON_PIN ) == 0 )   /* ainda premido */
        xSemaphoreGive( xButtonSem );
}

static void vButtonHandlerTask( void *pvParameters )
{
    const TickType_t xMaxBlock = pdMS_TO_TICKS( 5000 );
    for( ;; )
    {
        if( xSemaphoreTake( xButtonSem, xMaxBlock ) == pdPASS )
            ulButtonPressCounts++;
        else
            vPrintString( "Sem toques em 5 s\r\n" );
    }
}

int main( void )
{
    stdio_init_all();
    gpio_init( BUTTON_PIN );
    gpio_set_dir( BUTTON_PIN, GPIO_IN );
    gpio_pull_up( BUTTON_PIN );
    gpio_set_irq_enabled_with_callback( BUTTON_PIN,
        GPIO_IRQ_EDGE_FALL, true, vButtonCallback );
        
    xButtonSem = xSemaphoreCreateBinary();
    xDebounceTimer = xTimerCreate( "Deb", pdMS_TO_TICKS( 30 ), pdFALSE, 0, vDebounceCallback );

    xTaskCreate( vButtonHandlerTask, "Botao", 512, NULL, 3, NULL );

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
