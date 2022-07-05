cmake_minimum_required(VERSION 3.11)
project(peacock_test)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS "-Wall -Werror -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_FLAGS_RELEASE "-O3 -g")

include_directories(
    ./
    ../
)

if (NOT TARGET peacock)
    add_subdirectory(
        ${PROJECT_SOURCE_DIR}/../peacock/ 
        ${PROJECT_BINARY_DIR}/peacock
    )
    link_directories(
        ${PROJECT_BINARY_DIR}/peacock
    )

endif() 


link_libraries(
    peacock
)

# add the executable
add_executable(peacock_test
    test.c
)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_FLAGS_RELEASE "-O3 -g")

# add the executable
add_executable(peacock_test_cpp
    test.cpp
)
