cmake_minimum_required(VERSION 3.5)
project(unitree_sdk2_hzj VERSION 0.1.0)

# ---------------------------------------------------------------------------
# HZJ DDS provider
#   bundled  (default) — copied thirdparty CycloneDDS 0.10.2, same as upstream
#   external (opt-in)  — UNITREE_HZJ_DDS_ROOT; NOT a drop-in for ros2_hzj 11.0.1
# ---------------------------------------------------------------------------
set(UNITREE_HZJ_DDS_PROVIDER "bundled" CACHE STRING
    "DDS provider: bundled (Cyclone 0.10.2) or external")
set_property(CACHE UNITREE_HZJ_DDS_PROVIDER PROPERTY STRINGS bundled external)
set(UNITREE_HZJ_DDS_ROOT "" CACHE PATH
    "Root of an external CycloneDDS install (required when provider=external)")

option(BUILD_EXAMPLES "Build original helloworld examples" OFF)
option(BUILD_HZJ_SMOKE "Build HZJ provider smoke executable" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

message(STATUS "Current system architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "unitree_sdk2_hzj ${PROJECT_VERSION}  provider=${UNITREE_HZJ_DDS_PROVIDER}")

include("${CMAKE_CURRENT_LIST_DIR}/cmake/HzjDdsProvider.cmake")

if(UNITREE_HZJ_DDS_PROVIDER STREQUAL "bundled")
    unitree_hzj_assert_bundled_cyclone_0102()
    # Same imported ddsc / ddscxx targets as topsun-bot/unitree_sdk2 @ 9754cd15.
    add_subdirectory(thirdparty)
    set(UNITREE_HZJ_PROVIDER_DEFINE UNITREE_HZJ_DDS_PROVIDER_BUNDLED)
    set(UNITREE_HZJ_DDS_LIBDIR
        "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lib/${CMAKE_SYSTEM_PROCESSOR}")
elseif(UNITREE_HZJ_DDS_PROVIDER STREQUAL "external")
    unitree_hzj_import_external_cyclone()
    set(UNITREE_HZJ_PROVIDER_DEFINE UNITREE_HZJ_DDS_PROVIDER_EXTERNAL)
else()
    message(FATAL_ERROR
        "UNITREE_HZJ_DDS_PROVIDER must be 'bundled' or 'external' (got '${UNITREE_HZJ_DDS_PROVIDER}')")
endif()

set(UNITREE_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/${CMAKE_SYSTEM_PROCESSOR})
find_library(UNITREE_SDK_LIB unitree_sdk2 PATHS ${UNITREE_SDK_PATH} NO_DEFAULT_PATH)

if(NOT UNITREE_SDK_LIB)
    message(FATAL_ERROR "Unitree SDK library for the architecture is not found")
else()
    message(STATUS "Unitree SDK library found at: ${UNITREE_SDK_LIB}")
endif()

find_package(Threads REQUIRED)

add_library(unitree_sdk2 STATIC IMPORTED GLOBAL)
set_target_properties(unitree_sdk2 PROPERTIES
    IMPORTED_LOCATION ${UNITREE_SDK_LIB})
target_link_libraries(unitree_sdk2 INTERFACE ddsc ddscxx Threads::Threads)
target_include_directories(unitree_sdk2 INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_compile_definitions(unitree_sdk2 INTERFACE ${UNITREE_HZJ_PROVIDER_DEFINE})
if(UNITREE_HZJ_DDS_LIBDIR)
    # Bundled libddscxx has RUNPATH $ORIGIN/../lib (install layout), not
    # thirdparty/lib/<arch>. Use DT_RPATH (legacy, inherited) so the copied
    # side-by-side .so files resolve at runtime without LD_LIBRARY_PATH.
    target_link_options(unitree_sdk2 INTERFACE
        "-Wl,--disable-new-dtags"
        "-Wl,-rpath,${UNITREE_HZJ_DDS_LIBDIR}")
endif()

# HZJ alias: same link line plus an explicit name for this product.
add_library(unitree_sdk2_hzj INTERFACE)
target_link_libraries(unitree_sdk2_hzj INTERFACE unitree_sdk2)

if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(BUILD_HZJ_SMOKE)
    add_subdirectory(hzj/smoke)
endif()

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${UNITREE_SDK_LIB}
    DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES cmake/unitree_sdk2Targets.cmake
    DESTINATION lib/cmake/unitree_sdk2)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    unitree_sdk2ConfigVersion.cmake
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY ExactVersion)

configure_file(cmake/unitree_sdk2Config.cmake.in unitree_sdk2Config.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/unitree_sdk2Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/unitree_sdk2ConfigVersion.cmake"
    DESTINATION lib/cmake/unitree_sdk2)

write_basic_package_version_file(
    unitree_sdk2_hzjConfigVersion.cmake
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY ExactVersion)
configure_file(cmake/unitree_sdk2_hzjConfig.cmake.in unitree_sdk2_hzjConfig.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/unitree_sdk2_hzjConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/unitree_sdk2_hzjConfigVersion.cmake"
    DESTINATION lib/cmake/unitree_sdk2_hzj)
