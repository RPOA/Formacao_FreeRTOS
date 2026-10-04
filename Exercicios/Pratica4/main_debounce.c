#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"
#include "semphr.h"

#define BUTTON_PIN 15 /* ao GND, pull-up interno */

#define vPrintString( s ) printf( "%s", ( s ) )

static SemaphoreHandle_t xButtonSem;
static volatile uint32_t ulIsrCount, ulButtonPressCounts;

static void vButtonCallback( uint gpio,
                             uint32_t events )
{
    BaseType_t xWoken = pdFALSE;
    ulIsrCount++;
    xSemaphoreGiveFromISR( xButtonSem, &xWoken );
    portYIELD_FROM_ISR( xWoken );
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
        
    // xButtonSem = xSemaphoreCreateBinary();
    xButtonSem = xSemaphoreCreateCounting( 10, 0 ); /* 10 toques no máximo */
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
