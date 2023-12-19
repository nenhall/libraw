cmake_minimum_required(VERSION 3.26)
project(PixRaw)

set(CMAKE_CXX_STANDARD 17)

find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
    # 包括zlib头文件目录
    message(STATUS "ZLIB被成功找到：${ZLIB_INCLUDE_DIRS}")
    message(STATUS ${ZLIB_LIBRARIES})
    include_directories(${ZLIB_INCLUDE_DIRS})
else ()
    message(STATUS "ZLIB没有找到")
endif ()

include_directories(${PROJECT_SOURCE_DIR}/libraw
#        ${PROJECT_SOURCE_DIR}/libraw/src
)

file(GLOB SRC_FILES
        "${PROJECT_SOURCE_DIR}/libraw/src/decoders/*.h"
        "${PROJECT_SOURCE_DIR}/libraw/src/decoders/*.cpp"
        "${PROJECT_SOURCE_DIR}/libraw/src/demosaic/*.h"
        "${PROJECT_SOURCE_DIR}/libraw/src/demosaic/*.cpp"
        "${PROJECT_SOURCE_DIR}/libraw/src/integration/*.h"
        "${PROJECT_SOURCE_DIR}/libraw/src/integration/*.cpp"
        "${PROJECT_SOURCE_DIR}/libraw/src/metadata/*.h"
        "${PROJECT_SOURCE_DIR}/libraw/src/metadata/*.cpp"
        "${PROJECT_SOURCE_DIR}/libraw/src/postprocessing/*.h"
        "${PROJECT_SOURCE_DIR}/libraw/src/postprocessing/*.cpp"
        "${PROJECT_SOURCE_DIR}/libraw/src/preprocessing/*.h"
        "${PROJECT_SOURCE_DIR}/libraw/src/preprocessing/*.cpp"
        "${PROJECT_SOURCE_DIR}/libraw/src/tables/*.h"
        "${PROJECT_SOURCE_DIR}/libraw/src/tables/*.cpp"
        "${PROJECT_SOURCE_DIR}/libraw/src/utils/*.h"
        "${PROJECT_SOURCE_DIR}/libraw/src/utils/*.cpp"
        "${PROJECT_SOURCE_DIR}/libraw/src/write/*.h"
        "${PROJECT_SOURCE_DIR}/libraw/src/write/*.cpp"
        "${PROJECT_SOURCE_DIR}/libraw/src/x3f/*.h"
        "${PROJECT_SOURCE_DIR}/libraw/src/x3f/*.cpp"
        "${PROJECT_SOURCE_DIR}/libraw/src/*.cpp"
)

# 创建一个忽略包含_ph的文件的列表
set(SOURCE_FILES_FILTERED "")
foreach(FILE ${SRC_FILES})
    if(NOT FILE MATCHES "_ph")
        list(APPEND SOURCE_FILES_FILTERED ${FILE})
    endif()
endforeach()

add_library(PixRaw SHARED ${SOURCE_FILES_FILTERED} library.cpp)
add_executable(PixRawTest ${SOURCE_FILES_FILTERED} library.cpp)

target_link_libraries(PixRaw PRIVATE ${ZLIB_LIBRARIES})
target_link_libraries(PixRawTest PRIVATE ${ZLIB_LIBRARIES})
