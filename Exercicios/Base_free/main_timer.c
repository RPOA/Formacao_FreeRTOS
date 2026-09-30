#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"
#include "queue.h" /* ou semphr.h… */
#include "timers.h"

static QueueHandle_t xLedQueue;
static TimerHandle_t xSenderTimer;

/* Corre no contexto da timer daemon task: nunca bloquear aqui */
static void vSenderTimerCallback( TimerHandle_t xTimer )
{
    static uint32_t ulCount = 0;
    ( void ) xTimer;
    ulCount++;
    xQueueSend( xLedQueue, &ulCount, 0 );
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

    /* Auto-reload: dispara a cada 500 ms sem precisar de ser rearmado */
    xSenderTimer = xTimerCreate( "Sender", pdMS_TO_TICKS( 500 ),
                                 pdTRUE, NULL, vSenderTimerCallback );
    configASSERT( xSenderTimer != NULL );
    xTimerStart( xSenderTimer, 0 );

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
