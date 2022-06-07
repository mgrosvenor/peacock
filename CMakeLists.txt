cmake_minimum_required(VERSION 3.23)
include(ExternalProject)

project(peacock_all LANGUAGES C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(peacock)
add_subdirectory(tests)

ExternalProject_Add(peacock_device
    PREFIX peacock_device    
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/peacock_device
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG> --parallel
    BINARY_DIR ${CMAKE_BINARY_DIR}/peacock_device
    INSTALL_DIR ${CMAKE_BINARY_DIR}/peacock_device
    INSTALL_COMMAND make install
    

)
