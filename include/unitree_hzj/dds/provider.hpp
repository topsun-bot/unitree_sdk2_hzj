// Copyright (c) 2026 topsun-bot. HZJ rewrite of the unitree_sdk2 DDS integration layer.
//
// Thin provider abstraction. The default path (BundledCyclone010) links the
// copied thirdparty CycloneDDS 0.10.2 exactly like upstream unitree_sdk2.
// ExternalCyclone is opt-in and is NOT a drop-in for ros2_hzj's Cyclone 11.0.1.
#pragma once

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
    const char* cyclone_version;  // "0.10.2" on bundled; nullptr if unknown
    bool is_default;
    bool drop_in_for_ros2_hzj;    // always false in this first cut
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

}  // namespace dds
}  // namespace unitree_hzj
