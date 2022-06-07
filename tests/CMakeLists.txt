cmake_minimum_required(VERSION 3.13)
project(peacock_test)
set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-Wall -Werror -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_FLAGS_RELEASE "-O3 -g")

include_directories(
    ./
    ../host/include
)

link_directories(
    ${peacock_BINARY_DIR}/host/src
)

link_libraries(
    libpeacock_host
)

# add the executable
add_executable(peacock_test
    test.c
)
