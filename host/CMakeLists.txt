cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# set the project name
project(peacock_host)

set(CMAKE_C_FLAGS "-Wall -Werror -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_FLAGS_RELEASE "-O3 -g")

# add the executable
add_executable(peacock_test    
    peacock_host_msg.c
    peacock_host_err.c
    peacock_host_gpio.c
    peacock_host_pins.c
    peacock_host_pwm.c
    peacock_host_util.c
    serial.c 
    ../msg/msg.c
    peacock_host.c
    test.c
)
