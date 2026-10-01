#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"
#include "semphr.h"

#define BUTTON_PIN 15 /* ao GND, pull-up interno */
#define vPrintString( s ) printf( "%s", ( s ) )
#define vPrintStringAndNumber( s, n ) \
    printf( "%s %lu\r\n", ( s ), ( unsigned long )( n ) )

void vApplicationTickHook( void ){}

static QueueHandle_t xIntegerQueue, xStringQueue;

static void vIntegerGenerator( void *pvParameters )
{
    TickType_t xLastWake = xTaskGetTickCount();
    uint32_t ulValue = 0;
    for( ;; )
    {
        vTaskDelayUntil( &xLastWake, pdMS_TO_TICKS( 200 ) );
        for( int i = 0; i < 5; i++, ulValue++ )
            xQueueSendToBack( xIntegerQueue, &ulValue, 0 ); 
    }
}

static void vStringPrinter( void *pvParameters )
{
    char *pcString;
    for( ;; )
    {
        xQueueReceive( xStringQueue, &pcString, portMAX_DELAY );
        vPrintString( pcString );
    }
}

static void vButtonISR( uint gpio, uint32_t events )
{
    BaseType_t xWoken = pdFALSE;
    uint32_t ulNum;
    static const char *pcStrings[] = { "String 0\r\n",
        "String 1\r\n", "String 2\r\n", "String 3\r\n" };

    while( xQueueReceiveFromISR( xIntegerQueue, &ulNum, &xWoken )
           != errQUEUE_EMPTY )
        xQueueSendToBackFromISR( xStringQueue,
            &pcStrings[ ulNum & 0x03 ], &xWoken );

    portYIELD_FROM_ISR( xWoken );
}

int main( void )
{
    stdio_init_all();
    gpio_init( BUTTON_PIN );
    gpio_set_dir( BUTTON_PIN, GPIO_IN );
    gpio_pull_up( BUTTON_PIN );
    gpio_set_irq_enabled_with_callback( BUTTON_PIN,
        GPIO_IRQ_EDGE_FALL, true, vButtonISR );
    
    xIntegerQueue = xQueueCreate( 10, sizeof( uint32_t ) );
    xStringQueue = xQueueCreate( 10, sizeof( char * ) );
    xTaskCreate( vIntegerGenerator, "Integer Generator", 256, NULL, 1, NULL );
    xTaskCreate( vStringPrinter, "String Printer", 256, NULL, 2, NULL );

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
