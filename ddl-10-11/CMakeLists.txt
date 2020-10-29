cmake_minimum_required(VERSION 3.1)

project(main)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenCV_DIR /home/jin/下载/opencv-3.4.11/build)
find_package(OpenCV REQUIRED)

include_directories(/home/jin/下载/mindvision/include)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(main main.cpp)

target_link_libraries(main ${OpenCV_LIBS})

