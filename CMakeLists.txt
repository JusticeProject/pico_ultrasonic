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
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_logic_analyzer C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable (pico_ultrasonic 
				main.c
				ultrasonic_distance.c)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico_ultrasonic 1)
pico_enable_stdio_uart(pico_ultrasonic 0)

# invoke pio_asm to assemble the state machine programs
file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)
pico_generate_pio_header(pico_ultrasonic ${CMAKE_CURRENT_LIST_DIR}/ultrasonic_distance.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

# link the executable using the IR transmit and receive libraries
target_link_libraries(pico_ultrasonic PRIVATE
  			pico_stdlib
  			hardware_pio)

pico_add_extra_outputs(pico_ultrasonic)

