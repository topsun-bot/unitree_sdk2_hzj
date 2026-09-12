// Link-time smoke: original ChannelFactory + HZJ provider symbols resolve
// against the default BundledCyclone010 (or ExternalCyclone when selected).
// Does not call Init() — that needs a live DDS domain / NIC.

#include <unitree_hzj/unitree_hzj.hpp>

#include <iostream>

int main()
{
    const auto provider = unitree_hzj::dds::active_provider();
    std::cout << "unitree_sdk2_hzj " << unitree_hzj::kVersion << "\n";
    std::cout << "upstream " << unitree_hzj::kUpstreamCommit << "\n";
    std::cout << "provider " << provider.name << "\n";
    if (provider.cyclone_version)
    {
        std::cout << "cyclone " << provider.cyclone_version << "\n";
    }
    std::cout << "drop_in_for_ros2_hzj " << (provider.drop_in_for_ros2_hzj ? "true" : "false")
              << "\n";
    std::cout << "robot_wire_interop_proven "
              << (provider.robot_wire_interop_proven ? "true" : "false") << "\n";
    std::cout << "topic " << unitree_hzj::topics::kLowCmd << " " << unitree_hzj::topics::kLowState
              << "\n";

    auto* factory = unitree::robot::ChannelFactory::Instance();
    if (!factory)
    {
        return 1;
    }

    void (*init_fn)(int32_t, const std::string&) = &unitree::robot::ChannelFactoryInitialize;
    (void)init_fn;
    (void)factory;
    return 0;
}
