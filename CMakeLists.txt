cmake_minimum_required(VERSION 3.13)

# Pull in Pico SDK from git submodule
include(pico_sdk_import.cmake)

project(PokeControllerForPico C CXX ASM)

# Export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initializes the Pico SDK
pico_sdk_init()

# Add the source files to the executable
add_executable(PokeControllerForPico
    src/main.cpp
    src/PokeControllerForPico_Func.cpp
    src/CustomKeyboard.cpp
    src/usb_descriptors.c
)

# Add the directory with our headers to the include path
target_include_directories(PokeControllerForPico PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src
)

# Link to Pico libraries
target_link_libraries(PokeControllerForPico PRIVATE
    pico_stdlib
    hardware_uart
    hardware_clocks
)

# Link TinyUSB for all builds to make headers available
target_link_libraries(PokeControllerForPico PRIVATE tinyusb_device)

# For the waveshare board, add PIO USB and define the test flag
if (PICO_BOARD STREQUAL "waveshare_rp2350_usb_a")
    target_compile_definitions(PokeControllerForPico PUBLIC USE_PIO_USB_TEST_ONLY)
    add_subdirectory(lib/Pico-PIO-USB)
    target_link_libraries(PokeControllerForPico PRIVATE pico_pio_usb)
endif()

# Common TinyUSB configuration for descriptor headers
target_compile_definitions(PokeControllerForPico INTERFACE
    TUSB_CONFIG_FILE='${CMAKE_CURRENT_LIST_DIR}/src/tusb_config.h'
)

# Create .uf2 file
pico_add_extra_outputs(PokeControllerForPico)