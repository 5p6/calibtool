cmake_minimum_required(VERSION 3.15)

project(calib)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")


find_package(Boost REQUIRED COMPONENTS system thread program_options filesystem)
find_package(Ceres REQUIRED)
find_package(OpenCV 3 REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${CERES_INCLUDE_DIRS}
)
# lib build
## camera_models library
add_library(camera_models SHARED
    ${PROJECT_SOURCE_DIR}/include/camera_models/Camera.h
    ${PROJECT_SOURCE_DIR}/include/camera_models/CameraFactory.h
    ${PROJECT_SOURCE_DIR}/include/camera_models/CataCamera.h
    ${PROJECT_SOURCE_DIR}/include/camera_models/CostFunctionFactory.h
    ${PROJECT_SOURCE_DIR}/include/camera_models/EquidistantCamera.h
    ${PROJECT_SOURCE_DIR}/include/camera_models/PinholeCamera.h
    ${PROJECT_SOURCE_DIR}/include/camera_models/ScaramuzzaCamera.h
    ${PROJECT_SOURCE_DIR}/include/gpl/gpl.h
    ${PROJECT_SOURCE_DIR}/src/camera_models/Camera.cc
    ${PROJECT_SOURCE_DIR}/src/camera_models/CameraFactory.cc
    ${PROJECT_SOURCE_DIR}/src/camera_models/CataCamera.cc
    ${PROJECT_SOURCE_DIR}/src/camera_models/CostFunctionFactory.cc
    ${PROJECT_SOURCE_DIR}/src/camera_models/EquidistantCamera.cc
    ${PROJECT_SOURCE_DIR}/src/camera_models/PinholeCamera.cc
    ${PROJECT_SOURCE_DIR}/src/camera_models/ScaramuzzaCamera.cc
    ${PROJECT_SOURCE_DIR}/src/gpl/gpl.cc
)
target_link_libraries(camera_models PRIVATE
    ${CERES_LIBRARIES}
    ${OpenCV_LIBS}
    ${Boost_SYSTEM_LIBRARY}
)

## calib library
add_library(calib SHARED
    ${PROJECT_SOURCE_DIR}/src/calib/CameraCalibration.cc
    ${PROJECT_SOURCE_DIR}/src/calib/StereoCameraCalibration.cc
    ${PROJECT_SOURCE_DIR}/src/calib/Transform.cc
    ${PROJECT_SOURCE_DIR}/src/gpl/EigenQuaternionParameterization.cc
    ${PROJECT_SOURCE_DIR}/include/gpl/EigenQuaternionParameterization.h
)
target_link_libraries(calib PRIVATE
    ${CERES_LIBRARIES}
    ${OpenCV_LIBS}
    ${Boost_SYSTEM_LIBRARY}
    camera_models
)

## board detect library
add_library(cbdetect SHARED
    ${PROJECT_SOURCE_DIR}/include/chessboard/TargetDetector.hpp
    ${PROJECT_SOURCE_DIR}/include/chessboard/Chessboard.h
    ${PROJECT_SOURCE_DIR}/include/chessboard/ChessboardCorner.h
    ${PROJECT_SOURCE_DIR}/include/chessboard/ChessboardQuad.h
    ${PROJECT_SOURCE_DIR}/include/chessboard/Spline.h
    ${PROJECT_SOURCE_DIR}/include/chessboard/CircleGridFinder.hpp
    ${PROJECT_SOURCE_DIR}/src/chessboard/TargetDetector.cc
    ${PROJECT_SOURCE_DIR}/src/chessboard/Chessboard.cc
    ${PROJECT_SOURCE_DIR}/src/chessboard/CircleGridFinder.cc
)
target_link_libraries(cbdetect PRIVATE
    ${OpenCV_LIBS}
    ${Boost_SYSTEM_LIBRARY}
)

# build example
## monocular
add_executable(monocalib ${PROJECT_SOURCE_DIR}/monocalib.cc)
target_link_libraries(monocalib
    camera_models
    calib
    cbdetect
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
)
## stereo
add_executable(stereocalib ${PROJECT_SOURCE_DIR}/stereocalib.cc)
target_link_libraries(stereocalib
    camera_models
    calib
    cbdetect
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
)