#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"
#include "semphr.h"

void vApplicationTickHook( void ){}

SemaphoreHandle_t xMutex;

/* só neste exercício: lenta e sem proteção */
static void vPrintString( const char *pcString )
{
    for( ; *pcString != 0; pcString++ )
    {
        putchar_raw( *pcString );
        busy_wait_us( 100 ); /* periférico lento */
    }
}

static void prvPrintTask( void *pvParameters )
{
    const char *pcString = ( const char * ) pvParameters;
    for( ;; )
    {
        xSemaphoreTake( xMutex, portMAX_DELAY );
        vPrintString( pcString );
        xSemaphoreGive( xMutex );
        vTaskDelay( pdMS_TO_TICKS( 2 ) ); /* < 1 linha */
    }
}

int main( void )
{
    stdio_init_all();
    
    xMutex = xSemaphoreCreateMutex();
    xTaskCreate( prvPrinttask, "Print1", 512, ( void * )
    "Task 1 ***************************************\r\n", 1, NULL );
    xTaskCreate( prvPrinttask, "Print2", 512, ( void * )
    "Task 2 |||||||||||||||||||||||||||||||||||||||\r\n", 2, NULL );

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
