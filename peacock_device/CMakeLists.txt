cmake_minimum_required(VERSION 3.23)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CPP_COMPILER arm-none-eabi-g++)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS "-march=armv6-m -Wall -Werror -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_FLAGS_RELEASE "-O3 -g")

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(peacock_dev)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

include_directories(
        ./
        ${PROJECT_SOURCE_DIR}./  
        ${PROJECT_SOURCE_DIR}/../common
)

add_executable(peacock_dev
        peacock_dev_msg.c
        peacock_dev_err.c
        peacock_dev_pins.c
        peacock_dev_gpio.c
        peacock_dev_pwm.c  
        peacock_dev.c       
)

target_compile_definitions(peacock_dev PUBLIC
        PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS=-1
        PICO_STDIO_DEFAULT_CRLF=0
)

if (NOT TARGET peacock_msg)
        add_subdirectory(
                ${PROJECT_SOURCE_DIR}/../common/peacock_msg ${PROJECT_BINARY_DIR}/peacock_msg
        )

        link_directories(
                ${PROJECT_BINARY_DIR}/peacock_msg
        )
endif() 

target_link_libraries(peacock_dev 
        pico_stdlib 
        hardware_pwm 
        hardware_gpio
        hardware_timer
        peacock_msg
)



# enable usb output, disable uart output
pico_enable_stdio_usb(peacock_dev 1)
pico_enable_stdio_uart(peacock_dev 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(peacock_dev)

install(TARGETS peacock_dev DESTINATION ${CMAKE_BINARY_DIR})
