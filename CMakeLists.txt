cmake_minimum_required(VERSION 3.10)
project(EmulatorJit)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #CLANGD FILES EXPORT

add_compile_definitions(_WIN32_WINNT=0x0601)

add_executable(emulator
    src/main.cpp
    src/raw_emulator.cpp
)

target_include_directories(emulator PRIVATE include)
