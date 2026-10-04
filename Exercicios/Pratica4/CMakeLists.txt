# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.1)
set(toolchainVersion RISCV_PICO_2_3_1_0)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Pratica3 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Set Project
set( FREERTOS_KERNEL_PATH
    ${CMAKE_CURRENT_LIST_DIR}/../../FreeRTOS-Kernel )
set( FREERTOS_PORT_PATH
    ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2350_RISC-V )
include( ${FREERTOS_PORT_PATH}/FreeRTOS_Kernel_import.cmake )

# Add executable. Default name is the project name, version 0.1

add_executable(Pratica3 main.c )

pico_set_program_name(Pratica3 "Pratica3")
pico_set_program_version(Pratica3 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Pratica3 0)
pico_enable_stdio_usb(Pratica3 0)
pico_enable_stdio_rtt( Pratica3 1 )


# Add the standard library to the build
target_link_libraries(Pratica3
        pico_stdlib
        FreeRTOS-Kernel-Heap4)

# Add the standard include files to the build
target_include_directories(Pratica3 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(Pratica3)

