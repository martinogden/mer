cmake_minimum_required(VERSION 3.15)
project(mer)

set(CMAKE_CXX_STANDARD 14)
#set(EXECUTABLE_OUTPUT_PATH ../bin)

include_directories(include lib)
file(GLOB SOURCE_FILES src/**/**/*.cpp src/**/*.cpp src/*.cpp lib/*.cpp)
add_executable(mer ${SOURCE_FILES})
