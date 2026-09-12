// Compiled against the Cyclone headers selected by CMake:
//   bundled  -> thirdparty/include (must be DDS_VERSION 0.10.2)
//   external -> UNITREE_HZJ_DDS_ROOT/include (any version; ABI UNPROVEN)
//
// This TU is the real compile-time switch. It is not header-only.

#include <dds/version.h>

#include <unitree_hzj/dds/provider.hpp>

#if !defined(UNITREE_HZJ_DDS_PROVIDER_EXTERNAL)
#if !(DDS_VERSION_MAJOR == 0 && DDS_VERSION_MINOR == 10 && DDS_VERSION_PATCH == 2)
#error "BundledCyclone010 must compile cyclone_link.cpp against thirdparty Cyclone 0.10.2"
#endif
#endif

namespace unitree_hzj
{
namespace dds
{

const char* linked_cyclone_version()
{
    return DDS_VERSION;
}

const char* linked_provider_name()
{
#if defined(UNITREE_HZJ_DDS_PROVIDER_EXTERNAL)
    return "ExternalCyclone";
#else
    return "BundledCyclone010";
#endif
}

}  // namespace dds
}  // namespace unitree_hzj
