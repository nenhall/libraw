cmake_minimum_required(VERSION 3.16)

project(PixRaw VERSION 1.0.0 LANGUAGES C CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 选项
option(PIX_RAW_BUILD_C_API "Build C API" OFF)
option(PIX_RAW_INSTALL "Generate install target" OFF)

# 依赖 LibRaw - 使用 FetchContent 下载到 third_party/LibRaw/src
include(FetchContent)

# 先清理可能的不完整下载
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/LibRaw/src/.git" AND
   NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/LibRaw/src/libraw")
    message(WARNING "检测到不完整的 LibRaw 源码，将重新下载")
    file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/LibRaw/src")
endif()

FetchContent_Declare(
    libraw_external
    GIT_REPOSITORY https://github.com/LibRaw/LibRaw.git
    GIT_TAG        0.22.0
    SOURCE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/third_party/LibRaw/src
)

# 禁用 LibRaw 的测试和示例
set(LIBRAW_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)

# 启用 OpenMP 支持以提升性能
set(LIBRAW_ENABLE_OPENMP ON CACHE BOOL "" FORCE)

# 启用 RawSpeed 支持以增强格式兼容性
set(LIBRAW_ENABLE_RAWSPEED ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(libraw_external)

# 删除 LibRaw 源码中的 .git 目录以避免 git 子模块问题
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/LibRaw/src/.git")
    message(STATUS "删除 LibRaw 源码中的 .git 目录")
    file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/LibRaw/src/.git")
endif()

# 添加 LibRaw 子目录（LibRaw 不原生支持 CMake，使用项目提供的构建配置）
add_subdirectory(third_party/LibRaw)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/LibRaw/src
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
