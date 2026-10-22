# Patch ao port RP2350_RISC-V

O port `RP2350_RISC-V` do FreeRTOS-Kernel (fork `raspberrypi/FreeRTOS-Kernel`, commit `4f7299d`)
tem um bug com `configNUMBER_OF_CORES 1`: em `freertos_risc_v_mtimer_interrupt_handler`
(`portASM.S`) o valor devolvido por `xTaskIncrementTick()` é apagado por `mv a0, s1`.
O tick nunca provoca troca de contexto, por isso uma task acordada por `vTaskDelay`/`vTaskDelayUntil`
fica pronta mas não corre.

Corrigido no PR (ainda aberto) https://github.com/FreeRTOS/FreeRTOS-Kernel-Community-Supported-Ports/pull/34
(o port foi migrado para esse repositório; o fork da Raspberry Pi já não é mantido).
O patch aqui é o mesmo do PR.

## Aplicar (a partir da raiz do FreeRTOS-Kernel)

    git apply ../Exercicios/Base_free/patches/RP2350_RISCV_tick_fix.patch
