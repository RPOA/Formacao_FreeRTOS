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

static TaskInfo_t xT1 = { 0, 1u << 5 }; /* GP5 */
static TaskInfo_t xT2 = { 0, 1u << 6 }; /* GP6 */
static uint32_t ulIdleCycleCount = 0;
static uint32_t ulPeriodicCount = 0;

static void vContinuousTask( void *pvParameters )
{
    TaskInfo_t *pxInfo = ( TaskInfo_t * ) pvParameters;
    vTaskSetApplicationTaskTag( xTaskGetIdleTaskHandle(),
            ( TaskHookFunction_t ) ( 1u << 7 ) );
            
    for( ;; )
    {
        pxInfo->ulCount++;
        // vTaskDelay( pdMS_TO_TICKS( 10 ) );
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

    /* GP5, GP6 e GP7 (idle) como saídas para o analisador lógico */
    const uint32_t ulPins = xT1.ulPin | xT2.ulPin | ( 1u << 7 );
    gpio_init_mask( ulPins );
    gpio_set_dir_out_masked( ulPins );
    gpio_clr_mask( ulPins );

    TaskHandle_t xH1, xH2;
    xTaskCreate( vContinuousTask, "Continuous1", 128,
             &xT1, 1, &xH1 );
    xTaskCreate( vContinuousTask, "Continuous2", 128,
             &xT2, 1, &xH2 );

    vTaskSetApplicationTaskTag( xH1, ( TaskHookFunction_t ) xT1.ulPin );
    vTaskSetApplicationTaskTag( xH2, ( TaskHookFunction_t ) xT2.ulPin );

    // xTaskCreate( vPeriodicTask, "Periodic", 128,
    //          NULL, 2, NULL );

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
