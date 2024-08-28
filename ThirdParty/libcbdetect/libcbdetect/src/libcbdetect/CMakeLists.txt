include_directories(${PROJECT_SOURCE_DIR}/include/libcbdetect)
aux_source_directory(. SOURCE_LIST)

# 生成动态库
add_library(dllcbdetect SHARED ${SOURCE_LIST})
target_link_libraries(dllcbdetect
    ${OpenCV_LIBS}
)