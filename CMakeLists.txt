cmake_minimum_required(VERSION 3.15)

project(pavlin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package( OpenCV REQUIRED )    
include_directories(${PROJECT_SOURCE_DIR}/includes ${OpenCV_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(src)

add_executable(app main.cpp)
target_link_libraries(app ${OpenCV_LIBS} src)

SET(GCC_COVERAGE_LINK_FLAGS    "-lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_videoio")
