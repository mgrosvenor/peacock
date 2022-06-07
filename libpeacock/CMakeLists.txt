cmake_minimum_required(VERSION 3.13)
project(libpeacock)
set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-Wall -Werror -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_FLAGS_RELEASE "-O3 -g")

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/../
    ${PROJECT_SOURCE_DIR}/../common/
)

add_subdirectory(
        ${PROJECT_SOURCE_DIR}/../common/peacock_msg ${PROJECT_BINARY_DIR}/peacock_msg
)
target_link_libraries(libpeacock libpeacock_msg)

# add the executable
add_library(peacock_host    
    peacock_err.c
    peacock_gpio.c
    peacock_msg.c
    peacock_pins.c
    peacock_util.c
    peacock_serial.c
    peacock.c    
)