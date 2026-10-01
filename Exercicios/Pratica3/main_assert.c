#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"
#include "queue.h"

void vApplicationTickHook( void ){}

static QueueHandle_t xQueue = NULL;

static void vSenderTask( void *pvParameters )
{
    uint32_t ulValue = 0;
    for( ;; )
    {
        xQueueSend( xQueue, &ulValue, 0 ); /* assert */
        vTaskDelay( pdMS_TO_TICKS( 100 ) );
    }
}

int main( void )
{
    stdio_init_all();

    /* xQueue = xQueueCreate( 5, sizeof( uint32_t ) ); */

    xTaskCreate( vSenderTask, "Sender", 256, NULL, 1, NULL );

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
