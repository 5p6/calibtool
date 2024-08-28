cmake_minimum_required(VERSION 3.15)
# 工具链
set(VCPKG_ROOT "F:/app/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT})

project(calib)

# lib
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_PATH ${PROJECT_SOURCE_DIR}/build/lib)

find_package(OpenCV REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/ThirdParty/libcbdetect/libcbdetect/include
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)


option(BUILD_SHARED "" ON)
if(BUILD_SHARED)
    message("build the shared lib")
    add_definitions(-DBUILD_SHARED)
endif()

# 构建库
add_library(Oneshot_calib SHARED
    src/calib.h
    src/calib.cc
    src/CameraConfig.h
    src/CameraConfig.cc
    src/config.h
)

# 链接
target_link_libraries(Oneshot_calib
    ${OpenCV_LIBS}
    ${PROJECT_SOURCE_DIR}/ThirdParty/libcbdetect/libcbdetect/lib/Release/dllcbdetect.lib
    ${PROJECT_SOURCE_DIR}/ThirdParty/libcbdetect/libcbdetect/lib/Release/dllcbdetect.dll
)



# 生成示例代码
add_executable(example_cbdetect
    example_cbdetect.cc
)
add_executable(example_calib
    example_calib.cc
)

# 链接
target_link_libraries(example_cbdetect
    ${OpenCV_LIBS}
    Oneshot_calib
)
target_link_libraries(example_calib
    ${OpenCV_LIBS}
    Oneshot_calib
)

install(FILES ${PROJECT_SOURCE_DIR}/ThirdParty/libcbdetect/libcbdetect/lib/Release/dllcbdetect.dll DESTINATION ${PROJECT_SOURCE_DIR}/build/Release)
install(FILES ${PROJECT_SOURCE_DIR}/build/lib/Release/Oneshot_calib.dll DESTINATION ${PROJECT_SOURCE_DIR}/build/Release)

