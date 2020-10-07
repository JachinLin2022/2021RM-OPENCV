cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project (2021_OPENCV_task1)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall") 

include_directories(
	                    /usr/local/include/opencv4
			)
link_directories(
	                 /usr/local/lib
			 )
file(GLOB SRCS
	     ${PROJECT_SOURCE_DIR}/main.cpp
	     )


add_executable (task1 main.cpp)

set_property(TARGET task1 PROPERTY CXX_STANDARD 11)
TARGET_LINK_LIBRARIES(task1 opencv_gapi opencv_imgcodecs opencv_videoio opencv_photo opencv_highgui opencv_core opencv_imgproc opencv_video ${EXTERNAL_LIBS})


