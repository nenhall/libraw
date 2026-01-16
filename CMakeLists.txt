cmake_minimum_required(VERSION 3.16)

project(PixRaw VERSION 1.0.0 LANGUAGES C CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 选项
option(PIX_RAW_BUILD_C_API "Build C API" OFF)
option(PIX_RAW_INSTALL "Generate install target" OFF)

# 依赖 LibRaw - 使用 FetchContent 下载
include(FetchContent)

FetchContent_Declare(
    libraw
    GIT_REPOSITORY https://github.com/LibRaw/LibRaw.git
    GIT_TAG        0.22.0
    SOURCE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/third_party/LibRaw/src
)

FetchContent_MakeAvailable(libraw)

# === 核心库 (C++) ===
add_library(PixRaw STATIC
    src/PixRaw.cpp
    src/RawImage.cpp
    src/RawMetadata.cpp
    src/RawData.cpp
    src/ImageAdjuster.cpp
)

target_include_directories(PixRaw PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libraw/src
)

target_link_libraries(PixRaw
    PUBLIC
        libraw
)

# MSVC specific
if(MSVC)
    target_compile_options(PixRaw PUBLIC /bigobj)
endif()

# 导出宏（如果是共享库）
# target_compile_definitions(RawProcessor PRIVATE RAW_PROCESSOR_EXPORTS)

# === 安装规则（可选）===
if(PIX_RAW_INSTALL)
    include(GNUInstallDirs)

    # 安装库
    install(TARGETS PixRaw
        EXPORT PixRawTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    # 安装头文件
    install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
    )

    # 导出 CMake 目标
    install(EXPORT PixRawTargets
        FILE PixRawTargets.cmake
        NAMESPACE PixRaw::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PixRaw
    )

    # 创建配置文件
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/PixRawConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/PixRawConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PixRaw
    )
endif()
