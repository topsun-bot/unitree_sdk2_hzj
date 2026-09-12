add_library(unitree_hzj_dds STATIC
    cyclone_link.cpp
    channel.cpp
)
target_include_directories(unitree_hzj_dds PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_compile_definitions(unitree_hzj_dds PUBLIC ${UNITREE_HZJ_PROVIDER_DEFINE})
target_link_libraries(unitree_hzj_dds PUBLIC ddsc ddscxx Threads::Threads)
# ChannelFactory::Init is resolved from the imported .a at the final link.
# Do not add a CMake cycle through the unitree_sdk2 target.
set_target_properties(unitree_hzj_dds PROPERTIES
    POSITION_INDEPENDENT_CODE ON)
