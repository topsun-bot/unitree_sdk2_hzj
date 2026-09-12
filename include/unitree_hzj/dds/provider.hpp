// Copyright (c) 2026 topsun-bot. HZJ rewrite of the unitree_sdk2 DDS integration layer.
//
// Compiled provider (see src/unitree_hzj/). The default path links the copied
// thirdparty CycloneDDS 0.10.2 exactly like upstream. ExternalCyclone is a
// real CMake compile/link switch (UNITREE_HZJ_DDS_PROVIDER=external), not a
// header-only #ifdef. It is NOT a drop-in for ros2_hzj Cyclone 11.0.1.
//
// ChannelFactory::Init / DdsFactoryModel::Init / DdsParticipant live in
// libunitree_sdk2.a (closed). This layer does not wrap or replace those
// symbols. ChannelFactoryInitialize is implemented here and then calls Init.
#pragma once

#include <cstdint>
#include <string>

#include <unitree_hzj/version.hpp>

namespace unitree_hzj
{
namespace dds
{

enum class ProviderKind
{
    BundledCyclone010,
    ExternalCyclone
};

struct ProviderInfo
{
    ProviderKind kind;
    const char* name;
    const char* cyclone_version;  // "0.10.2" on bundled; nullptr if unknown at compile time
    bool is_default;
    bool drop_in_for_ros2_hzj;    // always false
    bool robot_wire_interop_proven;
};

#if defined(UNITREE_HZJ_DDS_PROVIDER_EXTERNAL)
inline constexpr ProviderKind kActiveProviderKind = ProviderKind::ExternalCyclone;
#else
inline constexpr ProviderKind kActiveProviderKind = ProviderKind::BundledCyclone010;
#endif

inline constexpr bool using_bundled_cyclone010()
{
    return kActiveProviderKind == ProviderKind::BundledCyclone010;
}

inline ProviderInfo active_provider()
{
#if defined(UNITREE_HZJ_DDS_PROVIDER_EXTERNAL)
    return ProviderInfo{
        ProviderKind::ExternalCyclone,
        "ExternalCyclone",
        nullptr,
        false,
        false,
        false,
    };
#else
    return ProviderInfo{
        ProviderKind::BundledCyclone010,
        "BundledCyclone010",
        "0.10.2",
        true,
        false,
        false,
    };
#endif
}

// Compiled in src/unitree_hzj/cyclone_link.cpp against the selected Cyclone
// include path (thirdparty 0.10.2, or UNITREE_HZJ_DDS_ROOT).
const char* linked_cyclone_version();
const char* linked_provider_name();

// ChannelFactoryInitialize implementation. Consults the compiled-in provider,
// then calls unitree::robot::ChannelFactory::Init inside libunitree_sdk2.a.
// Does not invent a hook inside the archive.
void initialize_channel_factory(int32_t domainId, const std::string& networkInterface);
void initialize_channel_factory(const std::string& configFileName);

}  // namespace dds
}  // namespace unitree_hzj
