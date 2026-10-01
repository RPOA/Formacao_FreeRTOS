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

> **Nota:** o `CMakeLists.txt` só compila o `main.c`. Para usar um dos exemplos, apaga o sufixo do nome do ficheiro que queres usar (por exemplo, `main_timer.c` → `main.c`), substituindo o `main.c` que lá está.

## Dependências

A pasta `FreeRTOS-Kernel` não estão incluídas neste repositório. Para compilar, clona o kernel na raiz do repositório:

```sh
git clone https://github.com/raspberrypi/FreeRTOS-Kernel.git
```

O `CMakeLists.txt` de `Base_free` procura o kernel em `../../FreeRTOS-Kernel`.

## Compilar

Abre a pasta do exercício no VS Code com a extensão Raspberry Pi Pico e usa **Compile Project**. Também podes compilar na linha de comandos:

```sh
cd Exercicios/Base_free
mkdir build && cd build
cmake ..
ninja
```
