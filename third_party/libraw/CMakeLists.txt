cmake_minimum_required(VERSION 3.16)

project(libraw VERSION 0.21.3 LANGUAGES C CXX)

# Option to build shared or static library
option(LIBRAW_BUILD_SHARED "Build shared library" OFF)
option(LIBRAW_ENABLE_EXAMPLES "Build examples" OFF)
option(LIBRAW_ENABLE_OPENMP "Enable OpenMP" OFF)
option(LIBRAW_ENABLE_RAWSPEED "Enable RawSpeed" OFF)
option(LIBRAW_ENABLE_DEMOSAIC_PACK_GPL2 "Enable demosaic pack GPL2" OFF)
option(LIBRAW_ENABLE_DEMOSAIC_PACK_GPL3 "Enable demosaic pack GPL3" OFF)

# Set library type
if(LIBRAW_BUILD_SHARED)
    set(LIBRAW_TYPE SHARED)
else()
    set(LIBRAW_TYPE STATIC)
endif()

# Collect all source files
file(GLOB_RECURSE LIBRAW_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# Exclude samples, test files and problematic files
list(FILTER LIBRAW_SOURCES EXCLUDE REGEX ".*samples.*")
list(FILTER LIBRAW_SOURCES EXCLUDE REGEX ".*RawSpeed.*")
list(FILTER LIBRAW_SOURCES EXCLUDE REGEX ".*mediumformat\\.cpp.*")
list(FILTER LIBRAW_SOURCES EXCLUDE REGEX ".*_ph\\.cpp.*")

# Create stub file for missing functions (replaces problematic mediumformat.cpp)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/metadata/mediumformat_stub.cpp")
    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/src/metadata/mediumformat_stub.cpp"
"/* Stub functions for LibRaw - replaces problematic mediumformat.cpp */

#define LIBRAW_LIBRARY_BUILD
#include \"libraw/libraw.h\"
#include \"internal/dcraw_defs.h\"

void LibRaw::parse_phase_one(int base)
{
    (void)base;
}

void LibRaw::parse_mos(INT64 offset)
{
    (void)offset;
}
")
endif()
list(APPEND LIBRAW_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/metadata/mediumformat_stub.cpp")

# Create the library target
add_library(libraw ${LIBRAW_TYPE} ${LIBRAW_SOURCES})

# Set library properties
set_target_properties(libraw PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME ${PROJECT_NAME}
)

# Define LIBRAW_NODLL for static builds
if(NOT LIBRAW_BUILD_SHARED)
    target_compile_definitions(libraw PUBLIC LIBRAW_NODLL)
endif()

# MSVC specific: use /bigobj to handle large object files
if(MSVC)
    target_compile_options(libraw PRIVATE /bigobj)
endif()

# Include directories
target_include_directories(libraw PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/libraw>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Install rules
include(GNUInstallDirs)

install(TARGETS libraw
    EXPORT libraw-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/libraw/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libraw
    FILES_MATCHING PATTERN "*.h"
)

# Install internal headers (for library build only)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/internal/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/internal
    FILES_MATCHING PATTERN "*.h"
)

# Create and install export targets
install(EXPORT libraw-targets
    FILE libraw-targets.cmake
    NAMESPACE libraw::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libraw
)

# Create config file for find_package()
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/libraw-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/libraw-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libraw
)
