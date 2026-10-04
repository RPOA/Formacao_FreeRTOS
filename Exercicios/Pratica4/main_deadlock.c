#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static SemaphoreHandle_t xMutexUart, xMutexI2C;
static volatile uint32_t ulCountA, ulCountB;      /* ver no debugger */

uint64_t ullGetRunTimeCounter( void ){}

static void vTaskA( void *pv )      /* prioridade 2 */
{
    for( ;; )
    {
        xSemaphoreTake( xMutexUart, portMAX_DELAY );
        vTaskDelay( 1 );                          /* abre a janela: B corre aqui */
        xSemaphoreTake( xMutexI2C, portMAX_DELAY );
        ulCountA++;                               /* usar os dois recursos */
        printf( "A: %lu\r\n", ( unsigned long ) ulCountA );
        xSemaphoreGive( xMutexI2C );
        xSemaphoreGive( xMutexUart );
        vTaskDelay( pdMS_TO_TICKS( 100 ) );
    }
}

static void vTaskB( void *pv )      /* prioridade 2 */
{
    for( ;; )
    {
        xSemaphoreTake( xMutexI2C, portMAX_DELAY );   /* ordem inversa! */
        vTaskDelay( 1 );
        xSemaphoreTake( xMutexUart, portMAX_DELAY );
        ulCountB++;
        printf( "B: %lu\r\n", ( unsigned long ) ulCountB );
        xSemaphoreGive( xMutexUart );
        xSemaphoreGive( xMutexI2C );
        vTaskDelay( pdMS_TO_TICKS( 100 ) );
    }
}

/* Sinal de vida: continua a piscar mesmo com A e B presas */
static void vLedTask( void *pv )    /* prioridade 1 */
{
    for( ;; )
    {
        gpio_xor_mask( 1u << PICO_DEFAULT_LED_PIN );
        vTaskDelay( pdMS_TO_TICKS( 500 ) );
    }
}

int main( void )
{
    stdio_init_all();
    gpio_init( PICO_DEFAULT_LED_PIN );
    gpio_set_dir( PICO_DEFAULT_LED_PIN, GPIO_OUT );

    xMutexUart = xSemaphoreCreateMutex();
    xMutexI2C  = xSemaphoreCreateMutex();
    vQueueAddToRegistry( xMutexUart, "UART" );    /* nomes no debugger (opcional) */
    vQueueAddToRegistry( xMutexI2C,  "I2C" );

    xTaskCreate( vTaskA,   "A",   512, NULL, 2, NULL );
    xTaskCreate( vTaskB,   "B",   512, NULL, 2, NULL );
    xTaskCreate( vLedTask, "LED", 256, NULL, 1, NULL );

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