/*
 * FreeRTOSConfig.h mínimo - Sessão prática 1, primeiro exercício
 * Pico 2 (RP2350), RISC-V, port RP2350_RISC-V, um só core.
 * Tudo o que não está aqui fica no valor por omissão do FreeRTOS.h.
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* Obrigatórios (o FreeRTOS.h dá erro se faltarem) */
#define configUSE_PREEMPTION            1
#define configTICK_RATE_HZ              1000
#define configMAX_PRIORITIES            8
#define configMINIMAL_STACK_SIZE        512             /* words */
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0
#define configTICK_TYPE_WIDTH_IN_BITS   TICK_TYPE_WIDTH_32_BITS

/* heap_4 (FreeRTOS-Kernel-Heap4 no CMake) */
#define configTOTAL_HEAP_SIZE           ( 64 * 1024 )

/* Hooks do passo 5 (definidos no main.c) */
#define configUSE_MALLOC_FAILED_HOOK    1
#define configCHECK_FOR_STACK_OVERFLOW  1

/* Software timers: exigidos pelo port RP2350 (interop com pico_sync usa
 * event groups + xTimerPendFunctionCallFromISR) */
#define configUSE_TIMERS                1
#define configTIMER_TASK_PRIORITY       ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH        10
#define configTIMER_TASK_STACK_DEPTH    configMINIMAL_STACK_SIZE
#define INCLUDE_xTimerPendFunctionCall  1
// #define configSUPPORT_PICO_SYNC_INTEROP   0
// #define configSUPPORT_PICO_TIME_INTEROP   0


/* Funções usadas no exercício */
#define INCLUDE_vTaskDelay              1
#define INCLUDE_xTaskDelayUntil         1               /* vTaskDelayUntil() */
#define INCLUDE_vTaskSuspend            1               /* portMAX_DELAY espera sem limite */
#define INCLUDE_uxTaskGetStackHighWaterMark 1           /* folga mínima de stack */

/* configASSERT() usado no main() */
#include <assert.h>
#define configASSERT( x )               assert( x )

#endif /* FREERTOS_CONFIG_H */