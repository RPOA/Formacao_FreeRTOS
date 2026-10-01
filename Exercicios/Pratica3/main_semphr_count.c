#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h" /* sempre primeiro */
#include "task.h"
#include "semphr.h"

#define BUTTON_PIN 15 /* ao GND, pull-up interno */
void vApplicationTickHook( void ){}

static SemaphoreHandle_t xCountingSemaphore;
static volatile uint32_t ulIsrTime;

/* corre na interrupção do GPIO */
static void vButtonISR( uint gpio, uint32_t events )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    /* três eventos na mesma interrupção */
    xSemaphoreGiveFromISR( xCountingSemaphore,
                           &xHigherPriorityTaskWoken );
    xSemaphoreGiveFromISR( xCountingSemaphore,
                           &xHigherPriorityTaskWoken );
    xSemaphoreGiveFromISR( xCountingSemaphore,
                           &xHigherPriorityTaskWoken );

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void vHandlerTask( void *pvParameters )
{
    for( ;; )
    {
        xSemaphoreTake( xCountingSemaphore, portMAX_DELAY );
        uint32_t ulUs = time_us_32() - ulIsrTime;
        printf( "Handler: %u us depois da ISR\r\n",
                ( unsigned ) ulUs );
    }
}

int main( void )
{
    stdio_init_all();
    gpio_init( BUTTON_PIN );
    gpio_set_dir( BUTTON_PIN, GPIO_IN );
    gpio_pull_up( BUTTON_PIN );
    gpio_set_irq_enabled_with_callback( BUTTON_PIN,
        GPIO_IRQ_EDGE_FALL, true, vButtonISR );
    
    xCountingSemaphore = xSemaphoreCreateCounting( 10, 0 );
    xTaskCreate( vHandlerTask, "Handler", 256, NULL, 3, NULL );

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
