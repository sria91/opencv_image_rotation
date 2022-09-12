cmake_minimum_required(VERSION 3.24)

project("opencv_image_rotation")

find_package(OpenCV REQUIRED core imgproc highgui)

include_directories(${ROOT_PATH}/src)

add_executable(${PROJECT_NAME} ./src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
