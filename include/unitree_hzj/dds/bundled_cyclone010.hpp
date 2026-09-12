// Copyright (c) 2026 topsun-bot.
// Default DDS provider: the CycloneDDS 0.10.2 binaries copied into thirdparty/.
#pragma once

#include <unitree_hzj/dds/provider.hpp>

#if !defined(UNITREE_HZJ_DDS_PROVIDER_EXTERNAL)
#include <dds/version.h>
static_assert(DDS_VERSION_MAJOR == 0 && DDS_VERSION_MINOR == 10 && DDS_VERSION_PATCH == 2,
              "BundledCyclone010 must compile against thirdparty CycloneDDS 0.10.2 "
              "(dds/version.h DDS_VERSION). Do not replace thirdparty/ with ros2_hzj "
              "vendor/CycloneDDS 11.0.1.");
#endif

namespace unitree_hzj
{
namespace dds
{

struct BundledCyclone010
{
    static constexpr ProviderKind kind = ProviderKind::BundledCyclone010;
    static constexpr const char* name = "BundledCyclone010";
    static constexpr const char* cyclone_version = "0.10.2";
    static constexpr bool is_default = true;
    // ros2_hzj v0.1.0 vendors Cyclone 11.0.1. Those .so files are not ABI-compatible
    // with this provider. Do not copy them into thirdparty/.
    static constexpr bool drop_in_for_ros2_hzj = false;
    // Wire interop with a physical Unitree robot is UNPROVEN for this HZJ tree.
    static constexpr bool robot_wire_interop_proven = false;

    static ProviderInfo info()
    {
        return ProviderInfo{kind, name, cyclone_version, is_default, drop_in_for_ros2_hzj,
                            robot_wire_interop_proven};
    }
};

}  // namespace dds
}  // namespace unitree_hzj
