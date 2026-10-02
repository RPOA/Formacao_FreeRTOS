#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"

void vTraceIn( void *pvTag ) 
{ 
    if( pvTag != NULL ) 
    {
        sio_hw->gpio_set = ( uint32_t ) pvTag;
    }
}

void vTraceOut( void *pvTag )
{ 
    if( pvTag != NULL ) 
    {
        sio_hw->gpio_clr = ( uint32_t ) pvTag;
    }
}

typedef struct
{
    volatile uint32_t ulCount; /* ver no debugger */
    uint32_t ulPin; /* máscara do pino */
} TaskInfo_t;

static TaskInfo_t xT1 = { 0, 1u << 2 }; /* GP2 */
static TaskInfo_t xT2 = { 0, 1u << 3 }; /* GP3 */
static uint32_t ulIdleCycleCount = 0;
static uint32_t ulPeriodicCount = 0;

static void vContinuousTask( void *pvParameters )
{
    TaskInfo_t *pxInfo = ( TaskInfo_t * ) pvParameters;
    vTaskSetApplicationTaskTag( NULL,
            ( TaskHookFunction_t ) pxInfo->ulPin );
    vTaskSetApplicationTaskTag( xTaskGetIdleTaskHandle(), 
            ( TaskHookFunction_t ) ( 1u << 6 ) ); 
    for( ;; )
    {
        pxInfo->ulCount++;
        vTaskDelay( pdMS_TO_TICKS( 10 ) );
    }
}

static void vPeriodicTask( void *pvParameters )
{
    TickType_t xLastWake = xTaskGetTickCount();
    for( ;; )
    {
        ulPeriodicCount++;
        vTaskDelayUntil( &xLastWake,
                         pdMS_TO_TICKS( 3 ) );
    }
}

void vApplicationIdleHook( void )
{
    ulIdleCycleCount++;
}

int main( void )
{
    stdio_init_all();

    xTaskCreate( vContinuousTask, "Continuous1", 128,
             &xT1, 1, NULL );
    xTaskCreate( vContinuousTask, "Continuous2", 128,
             &xT2, 1, NULL );
    xTaskCreate( vPeriodicTask, "Periodic", 128,
             NULL, 2, NULL );

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
