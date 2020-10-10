cmake_minimum_required(VERSION 3.5.1)
project(main)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIERD ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(OpenCV_DIR /home/wasx/opencv-3.4.9/build)
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(main main.cpp)
target_link_libraries(main ${OpenCV_LIBS})
