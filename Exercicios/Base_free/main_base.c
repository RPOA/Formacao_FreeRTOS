#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"
#include "queue.h" /* ou semphr.h… */

static QueueHandle_t xLedQueue;

static void vSenderTask( void *pvParameters )
{
    TickType_t xLastWake = xTaskGetTickCount();
    uint32_t ulCount = 0;
    for( ;; )
    {
        vTaskDelayUntil( &xLastWake,
                         pdMS_TO_TICKS( 500 ) );
        ulCount++;
        xQueueSend( xLedQueue, &ulCount, 0 );
    }
}

static void vLedTask( void *pvParameters )
{
    uint32_t ulReceived;
    for( ;; )
    {
        /* portMAX_DELAY: só retorna com dados */
        xQueueReceive( xLedQueue, &ulReceived,
                       portMAX_DELAY );
        gpio_xor_mask( 1u << PICO_DEFAULT_LED_PIN );
    }
}



int main( void )
{
    stdio_init_all();
    gpio_init( PICO_DEFAULT_LED_PIN );
    gpio_set_dir( PICO_DEFAULT_LED_PIN, GPIO_OUT );

    xLedQueue = xQueueCreate( 4, sizeof( uint32_t ) );
    configASSERT( xLedQueue != NULL );

    xTaskCreate( vLedTask, "LED", 256, NULL, 2, NULL );
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
