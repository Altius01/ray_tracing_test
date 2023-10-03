cmake_minimum_required(VERSION 3.15)

project(pavlin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package( OpenCV REQUIRED )    
include_directories(${PROJECT_SOURCE_DIR}/includes ${PROJECT_SOURCE_DIR}/lib ${OpenCV_INCLUDE_DIRS})
add_subdirectory(src)

add_executable(app main.cpp)
target_link_libraries(app src ${OpenCV_LIBS})
