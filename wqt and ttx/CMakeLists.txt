cmake_minimum_required(VERSION 3.1)
project(main)


set(OpenCV_DIR OpenCV_DIR /home/sp/opt/opencv-3.4.6/build)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(/home/sp/opt/opencv-3.4.6/include)
set(CMAKE_CXX_STANDARD 14)

add_executable(main main.cpp)

target_link_libraries(main ${OpenCV_LIBS})



