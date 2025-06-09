cmake_minimum_required(VERSION 3.10)


project(discocal)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# dependencies
find_package(OpenCV REQUIRED)
find_package(Eigen3)
find_package(OpenMP)


# lib
add_library(targetDetector SHARED
    ${PROJECT_SOURCE_DIR}/include/argparse.hpp
    ${PROJECT_SOURCE_DIR}/include/CCircleGridFinder.hpp
    ${PROJECT_SOURCE_DIR}/include/CMomentsTracker.h
    ${PROJECT_SOURCE_DIR}/include/CTargetDetector.h
    ${PROJECT_SOURCE_DIR}/include/utils.h
    ${PROJECT_SOURCE_DIR}/src/CTargetDetector.cpp 
    ${PROJECT_SOURCE_DIR}/src/CCircleGridFinder.cpp
)

if(OpenMP_CXX_FOUND)
target_link_libraries(targetDetector PRIVATE 
    OpenMP::OpenMP_CXX 
    ${OpenCV_LIBS}
)
endif()

target_include_directories(targetDetector PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
)


# main executable
add_executable(monodet monodet.cc)
target_link_libraries(monodet
    ${OpenCV_LIBS}
    targetDetector
)


add_executable(stereodet stereodet.cc)
target_link_libraries(stereodet
    ${OpenCV_LIBS}
    targetDetector
)