#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"
#include "queue.h"

#define vPrintString( s ) printf( "%s", ( s ) )

QueueHandle_t xPrintQueue;
static char *pcStringsToPrint[] = { "Task 1 *********\r\n",
    "Task 2 -----------…\r\n", "Tick hook ############…\r\n" };

static void prvStdioGatekeeperTask( void *pv )
{
    char *pcMsg;
    for( ;; )
    {
        xQueueReceive( xPrintQueue, &pcMsg,
                       portMAX_DELAY );
        vPrintString( pcMsg );
    }
}

void vApplicationTickHook( void )
{
    static int iCount = 0;
    if( ++iCount >= 200 ) {
        xQueueSendToFrontFromISR( xPrintQueue,
            &( pcStringsToPrint[ 2 ] ), NULL );
        iCount = 0; }
}

static void prvPrintTask( void *pvParameters )
{
    int iIndex = ( int ) pvParameters; /* 0 ou 1 */
    for( ;; )
    {
        xQueueSendToBack( xPrintQueue,
            &pcStringsToPrint[ iIndex ], 0 );
        vTaskDelay( pdMS_TO_TICKS( 10 ) ); 
    }
}

int main( void )
{
    stdio_init_all();
    
    xPrintQueue = xQueueCreate( 5, sizeof( char * ) );
    xTaskCreate( prvStdioGatekeeperTask, "Gatekeeper", 512, NULL, 1, NULL );
    xTaskCreate( prvPrintTask, "Print1", 512, ( void * ) 0, 2, NULL );
    xTaskCreate( prvPrintTask, "Print2", 512, ( void * ) 1, 3, NULL );

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
