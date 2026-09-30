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

/* ISR do GP15: roda no contexto da interrupção, NÃO pode bloquear */
static void vButtonISR( uint gpio, uint32_t events )
{
    static const uint32_t ulPeriods[] = { 500, 250, 100, 1000 };
    static uint32_t ulIndex = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if( gpio != BUTTON_PIN || !( events & GPIO_IRQ_EDGE_FALL ) )
    {
        return;
    }

    ulIndex = ( ulIndex + 1 ) % ( sizeof( ulPeriods ) / sizeof( ulPeriods[ 0 ] ) );
    xQueueSendFromISR( xPeriodQueue, &ulPeriods[ ulIndex ],
                       &xHigherPriorityTaskWoken );

    /* troca de contexto ao sair da ISR se acordou task de maior prioridade */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
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

    /* botão liga GP15 ao GND; fila já existe antes de habilitar a IRQ */
    gpio_init( BUTTON_PIN );
    gpio_set_dir( BUTTON_PIN, GPIO_IN );
    gpio_pull_up( BUTTON_PIN );
    gpio_set_irq_enabled_with_callback( BUTTON_PIN, GPIO_IRQ_EDGE_FALL,
                                        true, &vButtonISR );

    xTaskCreate( vLedTask, "LED", 256, NULL, 3, NULL );
    xTaskCreate( vSenderTask, "Sender", 256, NULL, 2, NULL );

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
