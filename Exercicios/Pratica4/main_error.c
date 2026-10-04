#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"

static QueueHandle_t xQ;
static volatile uint32_t ulTxCount, ulRxCount;
static volatile BaseType_t xError = pdFALSE;

uint64_t ullGetRunTimeCounter( void ){}

static void vProducerTask( void *pv ) /* prio 1 */
{
    uint32_t ulValue = 0;
    for( ;; )
    {
        xQueueSend( xQ, &ulValue, portMAX_DELAY );
        ulValue++;
        ulTxCount++;
        vTaskDelay( pdMS_TO_TICKS( 10 ) );
    }
}

static void vConsumerTask( void *pv ) /* prio 2 */
{
    uint32_t ulExpected = 0, ulValue;
    for( ;; )
    {
        xQueueReceive( xQ, &ulValue, portMAX_DELAY );
        if( ulValue != ulExpected++ )
        {
            xError = pdTRUE;
        }
        ulRxCount++;
    }
}

static void prvCheckTimerCallback( TimerHandle_t xT )
{
    static uint32_t ulLastTx, ulLastRx;
    if( ulTxCount == ulLastTx || ulRxCount == ulLastRx || xError )
        xTimerChangePeriod( xT, pdMS_TO_TICKS( 200 ), 0 ); /* erro */
    ulLastTx = ulTxCount;
    ulLastRx = ulRxCount;
    gpio_xor_mask( 1u << PICO_DEFAULT_LED_PIN );
}

int main( void )
{
    stdio_init_all();
    
    xQ = xQueueCreate( 5, sizeof( uint32_t ) );
    xTimerStart( xTimerCreate( "Check", pdMS_TO_TICKS( 1000 ),
                pdTRUE, NULL, prvCheckTimerCallback ), 0 );

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
