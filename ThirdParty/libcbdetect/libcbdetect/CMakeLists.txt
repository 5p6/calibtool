cmake_minimum_required(VERSION 3.15)
# 工具链
set(VCPKG_ROOT "F:/app/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT})


project(Libcbdetect)

# set
set(CMAKE_BUILD_TYPE Release)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_PATH ${PROJECT_SOURCE_DIR}/lib)

if (UNIX)
  set(CMAKE_CXX_COMPILER g++)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fopenmp")
  add_definitions(-DUNIX)
elseif (WIN32)
  add_definitions(-DWINDOWS)
  if (MSVC)
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -openmp")
    add_definitions("/EHsc")
  endif ()
endif ()

option(SHARED_LIB "Request build of shared libraries." ON)
if (SHARED_LIB)
  if (WIN32)
    add_definitions("-DIS_A_DLL")
  endif ()
  set(LIB_TYPE SHARED)
else ()
  set(LIB_TYPE STATIC)
endif ()

# find opencv and eigen
find_package(OpenCV)
find_package(Eigen3)

# header
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${EIGEN3_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)


add_subdirectory(${PROJECT_SOURCE_DIR}/src/libcbdetect)
