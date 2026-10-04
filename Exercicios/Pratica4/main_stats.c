#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"
#include "timers.h"
#include "semphr.h"

SemaphoreHandle_t xMutex;

uint64_t ullGetRunTimeCounter( void )
{
    return time_us_64();
}

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

static void prvStatsTask( void *pvParameters )
{
    static char cBuffer[ 512 ];
    for( ;; )
    {
        vTaskDelay( pdMS_TO_TICKS( 5000 ) );
        vTaskGetRunTimeStatistics( cBuffer, sizeof( cBuffer ) );
        printf( "\r\nTask Tempo(us) %%CPU\r\n%s", cBuffer );
        vTaskListTasks( cBuffer, sizeof( cBuffer ) );
        printf( "\r\nTask Estado Prio Stack Num\r\n%s", cBuffer );
    }
}


int main( void )
{
    stdio_init_all();
    
    xMutex = xSemaphoreCreateMutex();
    xTaskCreate( prvPrintTask, "Print1", 512, ( void * )
    "Task 1 ***************************************\r\n", 1, NULL );
    xTaskCreate( prvPrintTask, "Print2", 512, ( void * )
    "Task 2 |||||||||||||||||||||||||||||||||||||||\r\n", 2, NULL );
    xTaskCreate( prvStatsTask, "Stats", 1024, NULL, 1, NULL );

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
