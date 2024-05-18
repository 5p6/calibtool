cmake_minimum_required(VERSION 3.10)

project(camera_calibration)

### C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

### 设置不同的编译器
message(STATUS "CMake C++ compiler: ${CMAKE_CXX_COMPILER}")

## 链接库
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem program_options)
if(OpenCV_FOUND)
    message("OpenCV finded ")
else()
    message(FATAL_ERROR "OpenCV not find")
endif(OpenCV_FOUND)
    

### 添加包目录
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${OpenCV_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
)
### 生成库文件
add_library(calib SHARED
    ./src/calib.cc
    ./src/calib.h
    ./src/utility.cc
    ./src/utility.h
)

### 链接
target_link_libraries(calib
    ${OpenCV_LIBS}
)


# 示例代码
## 旧代码
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Example_old)
add_executable(example_calib ${PROJECT_SOURCE_DIR}/Example_old/example_calib.cc)

target_link_libraries(example_calib
    calib
)

add_executable(example_rectify ${PROJECT_SOURCE_DIR}/Example_old/example_rectify.cc)

target_link_libraries(example_rectify
    calib
)

## 新代码
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Example)
add_executable(monocalib ${PROJECT_SOURCE_DIR}/Example/monocalib.cc)
target_link_libraries(monocalib
    calib
    ${Boost_LIBRARIES}
)
add_executable(stereocalib ${PROJECT_SOURCE_DIR}/Example/stereocalib.cc)
target_link_libraries(stereocalib
    calib
    ${Boost_LIBRARIES}
)
add_executable(rectify ${PROJECT_SOURCE_DIR}/Example/rectify.cc)

target_link_libraries(rectify
    calib
    ${Boost_LIBRARIES}
)