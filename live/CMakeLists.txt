cmake_minimum_required(VERSION 2.8)
project(LearnOpenCV)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -march=native")
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(main main.cpp)
target_link_libraries(main ${OpenCV_LIBS})
