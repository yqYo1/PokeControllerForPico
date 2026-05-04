cmake_minimum_required(VERSION 3.13)

option(BUILD_TESTING "Build host tests" OFF)

if(NOT BUILD_TESTING)
    # Pull in Pico SDK from git submodule
    include(pico_sdk_import.cmake)
endif()

project(PokeControllerForPico C CXX ASM)

# Export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(NOT BUILD_TESTING)
    # Initializes the Pico SDK
    pico_sdk_init()

    # Add the source files to the executable
    add_executable(PokeControllerForPico
        src/main.cpp
        src/PokeControllerForPico_Func.cpp
        src/CustomKeyboard.cpp
        src/usb_descriptors.c
        src/serial_parser.cpp
    )

    # Add the directory with our headers to the include path
    target_include_directories(PokeControllerForPico PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/src
    )

    # Link to Pico libraries
    target_link_libraries(PokeControllerForPico
        pico_stdlib
        hardware_uart
    )

    # Link to TinyUSB
    target_link_libraries(PokeControllerForPico tinyusb_device)
    target_compile_definitions(PokeControllerForPico INTERFACE
        TUSB_CONFIG_FILE='${CMAKE_CURRENT_LIST_DIR}/src/tusb_config.h'
    )

    # Create .uf2 file
    pico_add_extra_outputs(PokeControllerForPico)
endif()

if(BUILD_TESTING)
    enable_testing()
    
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.8.0
    )
    FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
    )
    FetchContent_MakeAvailable(Catch2 json)
    
    add_executable(pico_parser_tests
        tests/test_parser.cpp
        src/serial_parser.cpp
    )
    target_include_directories(pico_parser_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(pico_parser_tests PRIVATE Catch2::Catch2WithMain nlohmann_json::nlohmann_json)
    
    include(Catch)
    catch_discover_tests(pico_parser_tests)
endif()
