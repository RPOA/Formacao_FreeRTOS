#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static SemaphoreHandle_t xLock;
#define PERIODO pdMS_TO_TICKS( 100 )

uint64_t ullGetRunTimeCounter( void ){}

/* Ocupa o CPU durante ulMs ms de execução (tick = 1 ms).
   Só conta os ticks em que a task está a correr: se for preemptada,
   o tempo parada não conta (ao contrário de busy_wait_ms). */
static void vWorkMs( uint32_t ulMs )
{
    while( ulMs-- )
    {
        TickType_t xT = xTaskGetTickCount();
        while( xTaskGetTickCount() == xT ) { }
    }
}

static void vLP( void *pv )      /* prioridade 1 */
{
    TickType_t xWake = xTaskGetTickCount();
    for( ;; )
    {
        xSemaphoreTake( xLock, portMAX_DELAY );
        vWorkMs( 20 );                       /* usa o recurso */
        xSemaphoreGive( xLock );
        vTaskDelayUntil( &xWake, PERIODO );
    }
}

static void vMP( void *pv )      /* prioridade 2 */
{
    TickType_t xWake = xTaskGetTickCount();
    for( ;; )
    {
        vTaskDelay( pdMS_TO_TICKS( 5 ) );    /* acorda depois de LP ter o recurso */
        vWorkMs( 50 );                       /* trabalho que não usa o recurso */
        vTaskDelayUntil( &xWake, PERIODO );
    }
}

static void vHP( void *pv )      /* prioridade 3 */
{
    TickType_t xWake = xTaskGetTickCount();
    for( ;; )
    {
        vTaskDelay( pdMS_TO_TICKS( 10 ) );   /* acorda a meio do trabalho de MP */
        uint32_t ulT0 = time_us_32();
        xSemaphoreTake( xLock, portMAX_DELAY );
        printf( "HP esperou %lu us\r\n",
                ( unsigned long )( time_us_32() - ulT0 ) );
        xSemaphoreGive( xLock );
        vTaskDelayUntil( &xWake, PERIODO );
    }
}

int main( void )
{
    stdio_init_all();

    xLock = xSemaphoreCreateBinary();        /* 1.ª corrida: semáforo binário */
    xSemaphoreGive( xLock );                 /* começa vazio: Give inicial */
    /* 2.ª corrida: xLock = xSemaphoreCreateMutex(); (sem o Give) */

    xTaskCreate( vLP, "LP", 512, NULL, 1, NULL );
    xTaskCreate( vMP, "MP", 512, NULL, 2, NULL );
    xTaskCreate( vHP, "HP", 512, NULL, 3, NULL );

    vTaskStartScheduler();
    for( ;; );
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