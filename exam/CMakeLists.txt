cmake_minimum_required(VERSION 3.1)
project(Temp)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 14)

add_executable(main main.cpp)

target_link_libraries(main ${OpenCV_LIBS})
