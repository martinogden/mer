cmake_minimum_required(VERSION 3.15)
project(mer)

set(CMAKE_CXX_STANDARD 11)
#set(EXECUTABLE_OUTPUT_PATH ../bin)

include_directories(inc lib)
file(GLOB SOURCE_FILES src/**/*.cpp src/*.cpp)
add_executable(mer ${SOURCE_FILES})
