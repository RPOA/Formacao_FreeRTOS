# Formação FreeRTOS

Exercícios da formação de FreeRTOS no Raspberry Pi Pico 2 (RP2350, RISC-V), usando o Pico SDK 2.3.1 e a extensão Raspberry Pi Pico para VS Code.

## Estrutura

- `Exercicios/Base_bare` – projeto base sem sistema operativo (bare-metal).
- `Exercicios/Base_free` – projeto base com FreeRTOS. Inclui vários exemplos:
  - `main_base.c` – pratica 1: projecto base - exercicio 3
  - `main_button.c` – pratica 1: exercicio 1
  - `main_heap_info.c` – pratica 1: exercicio 2
  - `main_isr.c` – pratica 1: exercicio 4
  - `main_timer.c` – pratica 1: exercicio 5
  - `main_stack.c` – pratica 1: exercicio 6
- `Exercicios/Pratica2` – tarefas, prioridades e trace por GPIO:
  - `main_trace.c` – pratica 2: exercicio 1, 2, 3, 4, 6
  - `main_prio.c` – pratica 2: exercicio 5
- `Exercicios/Pratica3` – queues, semáforos, mutexes e software timers:
  - `main_semphr.c` – pratica 3: exercicio 1
  - `main_semphr_count.c` – pratica 3: exercicio 2
  - `main_queue_isr.c` – pratica 3: exercicio 3
  - `main_mutex.c` – pratica 3: exercicio 4
  - `main_gatekeeper.c` – pratica 3: exercicio 5
  - `main_timers.c` – pratica 3: exercicio 6
  - `main_backlight.c` – pratica 3: exercicio 7
  - `main_assert.c` – pratica 3: exercicio 8
  - `main_greedy.c` – pratica 3: exercicio 9

> **Nota:** em `Base_free`, `Pratica2` e `Pratica3`, o `CMakeLists.txt` só compila o `main.c`. Para usar um dos exemplos, apaga o sufixo do nome do ficheiro que queres usar (por exemplo, `main_timer.c` → `main.c`), substituindo o `main.c` que lá estiver.

## Dependências

A pasta `FreeRTOS-Kernel` não está incluída neste repositório. Para compilar, clona o kernel na raiz do repositório:

```sh
git clone https://github.com/raspberrypi/FreeRTOS-Kernel.git
```

Os `CMakeLists.txt` de `Base_free`, `Pratica2` e `Pratica3` procuram o kernel em `../../FreeRTOS-Kernel`.

## Compilar

Abre a pasta do exercício no VS Code com a extensão Raspberry Pi Pico e usa **Compile Project**. Também podes compilar na linha de comandos:

```sh
cd Exercicios/Base_free
mkdir build && cd build
cmake ..
ninja
```
