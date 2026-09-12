// Copyright (c) 2026 topsun-bot.
// Opt-in DDS provider: an external CycloneDDS install pointed at by CMake
// UNITREE_HZJ_DDS_ROOT. This is NOT a drop-in replacement for BundledCyclone010.
//
// ros2_hzj v0.1.0 @ 218903b vendors Cyclone 11.0.1. That tree is a companion
// middleware workspace, not a substitute .so for unitree_sdk2's 0.10.2 ABI.
// Linking this SDK's prebuilt libunitree_sdk2.a (compiled against 0.10.2
// headers) to Cyclone 11.x is ABI-unsafe. Wire interop with a physical
// Unitree robot is UNPROVEN.
#pragma once

#include <unitree_hzj/dds/provider.hpp>

namespace unitree_hzj
{
namespace dds
{

struct ExternalCyclone
{
    static constexpr ProviderKind kind = ProviderKind::ExternalCyclone;
    static constexpr const char* name = "ExternalCyclone";
    static constexpr const char* cyclone_version = nullptr;
    static constexpr bool is_default = false;
    static constexpr bool drop_in_for_ros2_hzj = false;
    static constexpr bool robot_wire_interop_proven = false;

    static ProviderInfo info()
    {
        return ProviderInfo{kind, name, cyclone_version, is_default, drop_in_for_ros2_hzj,
                            robot_wire_interop_proven};
    }
};

}  // namespace dds
}  // namespace unitree_hzj
