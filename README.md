# Formação FreeRTOS

Exercícios da formação de FreeRTOS para o Raspberry Pi Pico 2 (RP2350, arquitetura RISC-V), utilizando o Pico SDK 2.3.1 e a extensão Raspberry Pi Pico para VS Code.

Este repositório reúne vários projetos de prática para explorar conceitos como tarefas, filas, semáforos, mutexes, timers, prioritização, debounce e diagnóstico de erros em sistemas multitarefa.

## Estrutura do projeto

- `Exercicios/Base_bare` – projeto base sem sistema operativo (bare-metal).
- `Exercicios/Base_free` – projeto base com FreeRTOS, com vários exemplos:
  - `main_base.c` – prática 1, exercício 3
  - `main_button.c` – prática 1, exercício 1
  - `main_heap_info.c` – prática 1, exercício 2
  - `main_isr.c` – prática 1, exercício 4
  - `main_timer.c` – prática 1, exercício 5
  - `main_stack.c` – prática 1, exercício 6
- `Exercicios/Pratica2` – tarefas, prioridades e trace por GPIO:
  - `main_trace.c` – prática 2, exercícios 1, 2, 3, 4 e 6
  - `main_prio.c` – prática 2, exercício 5
- `Exercicios/Pratica3` – filas, semáforos, mutexes e software timers:
  - `main_semphr.c` – prática 3, exercício 1
  - `main_semphr_count.c` – prática 3, exercício 2
  - `main_queue_isr.c` – prática 3, exercício 3
  - `main_mutex.c` – prática 3, exercício 4
  - `main_gatekeeper.c` – prática 3, exercício 5
  - `main_timers.c` – prática 3, exercício 6
  - `main_backlight.c` – prática 3, exercício 7
  - `main_assert.c` – prática 3, exercício 8
  - `main_greedy.c` – prática 3, exercício 9
- `Exercicios/Pratica4` – exercícios avançados de sincronização e diagnóstico:
  - `main_deferred.c` – prática 4, exercício 1
  - `main_debounce.c` – prática 4, exercício 2
  - `main_debounce_timer.c` – prática 4, exercício 2
  - `main_stats.c` – prática 4, exercício 3
  - `main_error.c` – prática 4, exercício 4
  - `main_invertion.c` – prática 4, exercício 5
  - `main_deadlock.c` – prática 4, exercício 6

## Requisitos

Para compilar os projetos, é necessário ter instalado o ambiente do Raspberry Pi Pico:

- VS Code
- Extensão Raspberry Pi Pico para VS Code
- Pico SDK 2.3.1
- Toolchain de compilação do Pico
- Git

## Dependências do FreeRTOS

A pasta `FreeRTOS-Kernel` deve existir na raiz do projeto para que os `CMakeLists.txt` consigam localizar o kernel. Caso ainda não exista, executa:

```sh
git clone https://github.com/raspberrypi/FreeRTOS-Kernel.git
```

Os ficheiros de configuração de `Base_free`, `Pratica2` e `Pratica3` procuram o kernel em `../../FreeRTOS-Kernel` a partir da pasta do exercício.

## Como alternar entre exercícios

Em `Base_free`, `Pratica2` e `Pratica3`, o `CMakeLists.txt` está preparado para compilar um ficheiro principal chamado `main.c`. Para testar um exercício concreto:

1. Abre a pasta do exercício.
2. Renomeia o ficheiro desejado para `main.c`.
3. Exemplo: `main_timer.c` → `main.c`.

> Esta troca é necessária porque cada projeto usa um único ficheiro de entrada do tipo `main.c`.

## Compilar

### Opção 1 – VS Code

Abre a pasta do exercício no VS Code com a extensão Raspberry Pi Pico e usa a opção **Compile Project**.

### Opção 2 – Linha de comandos

```sh
cd Exercicios/Base_free
mkdir -p build
cd build
cmake ..
ninja
```

Também podes repetir este processo para outras pastas de exercícios, como `Exercicios/Pratica2` ou `Exercicios/Pratica3`.

## Observações

- Alguns exercícios incluem ficheiros com nomes específicos para cada prática; o nome do ficheiro principal pode ser trocado conforme o exercício a testar.
- Verifica se o `PICO_SDK_PATH` e os caminhos do compilador estão corretamente definidos no ambiente antes de compilar.
- O repositório também inclui a pasta `FreeRTOS/`, que pode conter a documentação e fontes do projeto mais completo da família FreeRTOS.
