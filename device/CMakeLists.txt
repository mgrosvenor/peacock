cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(peacock_dev)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(peacock_dev
        peacock_dev_msg.c
        peacock_dev_err.c
        peacock_dev_pins.c
        peacock_dev_gpio.c
        peacock_dev_pwm.c
        ../msg/msg.c
        peacock_dev.c
)
target_compile_definitions(peacock_dev PUBLIC
        PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS=-1
        PICO_STDIO_DEFAULT_CRLF=0
)

target_link_libraries(peacock_dev 
        pico_stdlib 
        hardware_pwm 
        hardware_gpio
        hardware_timer
)

set(CMAKE_C_FLAGS "-march=armv6-m -Wall -Werror -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_FLAGS_RELEASE "-O3 -g")

# enable usb output, disable uart output
pico_enable_stdio_usb(peacock_dev 1)
pico_enable_stdio_uart(peacock_dev 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(peacock_dev)