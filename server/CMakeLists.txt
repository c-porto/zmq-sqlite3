cmake_minimum_required(VERSION 3.10)

set(this zmqserver)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_EXPORT_COMPILE_COMMANDS On)

project(${this} CXX)

file(GLOB SRC_FILES src/*.cc)

add_subdirectory(libs/libzmq)
add_subdirectory(libs/cppzmq)

add_executable(${this} main.cc ${SRC_FILES})
add_compile_options(-Wall -Wpedantic -O3)

target_include_directories(${this} INTERFACE ${CMAKE_SOURCE_DIR})

target_link_libraries(${this} cppzmq)

target_link_libraries(${this} sqlite3)
