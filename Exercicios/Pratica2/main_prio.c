#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"

void vApplicationIdleHook( void ){}

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

TaskHandle_t xT2Handle;

static void vTask2( void *pvParameters )
{
    TaskInfo_t *pxInfo = ( TaskInfo_t * ) pvParameters;
    uint32_t ulT2Count = 0;
    vTaskSetApplicationTaskTag( NULL,
            ( TaskHookFunction_t ) pxInfo->ulPin );
    vTaskSetApplicationTaskTag( xTaskGetIdleTaskHandle(), 
            ( TaskHookFunction_t ) ( 1u << 6 ) ); 

    for( ;; ) /* T2, prioridade 1 */
    {
        ulT2Count++;
        busy_wait_ms( 200 ); /* ocupada, sem bloquear */
        vTaskPrioritySet( NULL,
            uxTaskPriorityGet( NULL ) - 2 );
    }
}

static void vTask1( void *pvParameters )
{
    TaskInfo_t *pxInfo = ( TaskInfo_t * ) pvParameters;
    uint32_t ulT1Count = 0;
    vTaskSetApplicationTaskTag( NULL,
            ( TaskHookFunction_t ) pxInfo->ulPin );

    for( ;; ) /* T1, prioridade 2 */
    {
        ulT1Count++;
        busy_wait_ms( 200 ); /* ocupada, sem bloquear */
        vTaskPrioritySet( xT2Handle,
            uxTaskPriorityGet( NULL ) + 1 );
    }
}

int main( void )
{
    stdio_init_all();

    xTaskCreate( vTask1, "Task1", 256,
             &xT1, 2, NULL );
    xTaskCreate( vTask2, "Task2", 256,
             &xT2, 1, NULL );

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
