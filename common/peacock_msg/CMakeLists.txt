cmake_minimum_required(VERSION 3.13)
project(libpeacock_msg)
set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-Wall -Werror -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_FLAGS_RELEASE "-O3 -g")

# add the executable
add_library(peacock_msg    
    peacock_msg.c
)