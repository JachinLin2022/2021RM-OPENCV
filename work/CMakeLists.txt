CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

set(OpenCV_DIR /home/jin/下载/opencv-3.4.11/build)
find_package(OpenCV REQUIRED)
include_directories(/home/jin/下载/mindvision/include)
include_directories(${OpenCV_INCLUDE_DIRS}) 

PROJECT(main)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

ADD_EXECUTABLE(main main.cpp)

target_link_libraries(main ${OpenCV_LIBS})
