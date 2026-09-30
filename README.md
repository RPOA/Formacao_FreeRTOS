# Formação FreeRTOS

Exercícios da formação de FreeRTOS no Raspberry Pi Pico 2 (RP2350, RISC-V), usando o Pico SDK 2.3.1 e a extensão Raspberry Pi Pico para VS Code.

## Estrutura

- `Exercicios/Base_bare` – projeto base sem sistema operativo (bare-metal).
- `Exercicios/Base_free` – projeto base com FreeRTOS. Inclui vários exemplos:
  - `main_base.c` – tarefas básicas
  - `main_button.c` – leitura de botão
  - `main_isr.c` – interrupções
  - `main_timer.c` – software timers
  - `main_heap_info.c` – informação da heap

## Dependências

As pastas `FreeRTOS` e `FreeRTOS-Kernel` não estão incluídas neste repositório. Para compilar, clona o kernel na raiz do repositório:

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
