# HZJ DDS provider selection.
# bundled (default): thirdparty CycloneDDS 0.10.2, same imported targets as upstream.
# external: UNITREE_HZJ_DDS_ROOT. NOT a drop-in for ros2_hzj Cyclone 11.0.1.

function(unitree_hzj_assert_bundled_cyclone_0102)
    set(_ver_h "${PROJECT_SOURCE_DIR}/thirdparty/include/dds/version.h")
    if(NOT EXISTS "${_ver_h}")
        message(FATAL_ERROR
            "BundledCyclone010: missing ${_ver_h}. "
            "Default provider must use the copied thirdparty CycloneDDS 0.10.2.")
    endif()
    file(READ "${_ver_h}" _ver_txt)
    if(NOT _ver_txt MATCHES "DDS_VERSION[ \t]+\"0\\.10\\.2\"")
        message(FATAL_ERROR
            "BundledCyclone010 requires thirdparty/include/dds/version.h "
            "DDS_VERSION \"0.10.2\". Do not replace thirdparty/ with ros2_hzj "
            "vendor/CycloneDDS 11.0.1.")
    endif()
    message(STATUS "HZJ DDS provider: BundledCyclone010 (CycloneDDS 0.10.2)")
endfunction()

function(unitree_hzj_import_external_cyclone)
    if(NOT UNITREE_HZJ_DDS_ROOT)
        message(FATAL_ERROR
            "UNITREE_HZJ_DDS_PROVIDER=external requires UNITREE_HZJ_DDS_ROOT "
            "(root of an external CycloneDDS install with include/ and lib/).")
    endif()
    if(NOT IS_ABSOLUTE "${UNITREE_HZJ_DDS_ROOT}")
        get_filename_component(UNITREE_HZJ_DDS_ROOT "${UNITREE_HZJ_DDS_ROOT}"
            ABSOLUTE BASE_DIR "${CMAKE_BINARY_DIR}")
    endif()

    set(_inc "${UNITREE_HZJ_DDS_ROOT}/include")
    set(_ver_h "${_inc}/dds/version.h")
    if(NOT EXISTS "${_ver_h}")
        message(FATAL_ERROR
            "ExternalCyclone: ${UNITREE_HZJ_DDS_ROOT} has no include/dds/version.h")
    endif()

    set(_libdir "")
    foreach(_cand
            "${UNITREE_HZJ_DDS_ROOT}/lib/${CMAKE_SYSTEM_PROCESSOR}"
            "${UNITREE_HZJ_DDS_ROOT}/lib"
            "${UNITREE_HZJ_DDS_ROOT}/lib64")
        if(EXISTS "${_cand}/libddsc.so" AND EXISTS "${_cand}/libddscxx.so")
            set(_libdir "${_cand}")
            break()
        endif()
    endforeach()
    if(NOT _libdir)
        message(FATAL_ERROR
            "ExternalCyclone: could not find libddsc.so and libddscxx.so under "
            "${UNITREE_HZJ_DDS_ROOT}/lib[64][/<arch>]. "
            "ros2_hzj vendor/CycloneDDS 11.0.1 is NOT a drop-in .so layout for this SDK.")
    endif()

    message(WARNING
        "HZJ DDS provider: ExternalCyclone at ${UNITREE_HZJ_DDS_ROOT}. "
        "This is opt-in. Bundled Cyclone 0.10.2 and ros2_hzj Cyclone 11.0.1 are "
        "NOT ABI-compatible. Wire interop with a physical Unitree robot is UNPROVEN. "
        "Do not treat this as a drop-in replacement.")

    find_package(Threads REQUIRED)

    add_library(ddsc SHARED IMPORTED GLOBAL)
    set_target_properties(ddsc PROPERTIES
        IMPORTED_LOCATION "${_libdir}/libddsc.so"
        IMPORTED_NO_SONAME TRUE)
    target_link_libraries(ddsc INTERFACE Threads::Threads)
    target_link_directories(ddsc INTERFACE "${_libdir}")
    target_include_directories(ddsc INTERFACE
        $<BUILD_INTERFACE:${_inc}>
        $<INSTALL_INTERFACE:include>)

    add_library(ddscxx SHARED IMPORTED GLOBAL)
    set_target_properties(ddscxx PROPERTIES
        IMPORTED_LOCATION "${_libdir}/libddscxx.so"
        IMPORTED_NO_SONAME TRUE)
    target_link_libraries(ddscxx INTERFACE Threads::Threads)
    target_link_directories(ddscxx INTERFACE "${_libdir}")
    set(_cxx_inc "${_inc}")
    if(EXISTS "${_inc}/ddscxx")
        set(_cxx_inc "${_inc}/ddscxx")
    endif()
    target_include_directories(ddscxx INTERFACE
        $<BUILD_INTERFACE:${_inc}>
        $<BUILD_INTERFACE:${_cxx_inc}>
        $<INSTALL_INTERFACE:include>
        $<INSTALL_INTERFACE:include/ddscxx>)

    set(UNITREE_HZJ_DDS_LIBDIR "${_libdir}" PARENT_SCOPE)
endfunction()
