#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"
#include "queue.h" /* ou semphr.h… */

#define BUTTON_PIN 15

static QueueHandle_t xLedQueue;
static QueueHandle_t xPeriodQueue;

static void vSenderTask( void *pvParameters )
{
    TickType_t xLastWake = xTaskGetTickCount();
    uint32_t ulCount = 0;
    uint32_t ulPeriodMs = 500;
    for( ;; )
    {
        /* timeout 0: só consulta, não bloqueia o ritmo do pisca */
        xQueueReceive( xPeriodQueue, &ulPeriodMs, 0 );
        printf( "Sender: %u ms\n", ulPeriodMs );
        
        vTaskDelayUntil( &xLastWake,
                         pdMS_TO_TICKS( ulPeriodMs ) );
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

static void vButtonTask( void *pvParameters )
{
    static const uint32_t ulPeriods[] = { 500, 250, 100, 1000 };
    uint32_t ulIndex = 0;
    bool bLastPressed = false;

    gpio_init( BUTTON_PIN );
    gpio_set_dir( BUTTON_PIN, GPIO_IN );
    gpio_pull_up( BUTTON_PIN ); /* botão liga GP15 ao GND */

    for( ;; )
    {
        /* polling a cada 20 ms já faz o debounce */
        vTaskDelay( pdMS_TO_TICKS( 20 ) );
        bool bPressed = !gpio_get( BUTTON_PIN );

        if( bPressed && !bLastPressed ) /* só na borda de descida */
        {
            ulIndex = ( ulIndex + 1 ) % ( sizeof( ulPeriods ) / sizeof( ulPeriods[ 0 ] ) );
            xQueueSend( xPeriodQueue, &ulPeriods[ ulIndex ], 0 );
        }
        bLastPressed = bPressed;
    }
}

int main( void )
{
    stdio_init_all();
    gpio_init( PICO_DEFAULT_LED_PIN );
    gpio_set_dir( PICO_DEFAULT_LED_PIN, GPIO_OUT );

    xLedQueue = xQueueCreate( 4, sizeof( uint32_t ) );
    configASSERT( xLedQueue != NULL );

    xPeriodQueue = xQueueCreate( 4, sizeof( uint32_t ) );
    configASSERT( xPeriodQueue != NULL );

    xTaskCreate( vLedTask, "LED", 256, NULL, 3, NULL );
    xTaskCreate( vSenderTask, "Sender", 256, NULL, 2, NULL );
    xTaskCreate( vButtonTask, "Button", 256, NULL, 1, NULL );

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
