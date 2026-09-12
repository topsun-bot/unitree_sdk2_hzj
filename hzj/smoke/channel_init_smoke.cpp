// Link-time smoke: ChannelFactoryInitialize is a compiled symbol in
// libunitree_hzj_dds (provider -> closed ChannelFactory::Init).
// Does not call Init() — that needs a live DDS domain / NIC.

#include <unitree_hzj/unitree_hzj.hpp>

#include <cstring>
#include <iostream>

int main()
{
    const auto provider = unitree_hzj::dds::active_provider();
    const char* linked = unitree_hzj::dds::linked_cyclone_version();
    const char* linked_name = unitree_hzj::dds::linked_provider_name();

    std::cout << "unitree_sdk2_hzj " << unitree_hzj::kVersion << "\n";
    std::cout << "upstream " << unitree_hzj::kUpstreamCommit << "\n";
    std::cout << "provider " << provider.name << "\n";
    std::cout << "linked_provider " << (linked_name ? linked_name : "(null)") << "\n";
    std::cout << "linked_cyclone " << (linked ? linked : "(null)") << "\n";
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
    std::cout << "init_impl closed_static_lib\n";

    auto* factory = unitree::robot::ChannelFactory::Instance();
    if (!factory)
    {
        return 1;
    }

    void (*init_fn)(int32_t, const std::string&) = &unitree::robot::ChannelFactoryInitialize;
    void (*hzj_init)(int32_t, const std::string&) = &unitree_hzj::dds::initialize_channel_factory;
    if (init_fn == nullptr || hzj_init == nullptr)
    {
        return 1;
    }

    if (unitree_hzj::dds::using_bundled_cyclone010())
    {
        if (linked == nullptr || std::strcmp(linked, "0.10.2") != 0)
        {
            std::cerr << "FAIL: bundled path must link Cyclone 0.10.2, got "
                      << (linked ? linked : "(null)") << "\n";
            return 1;
        }
        if (linked_name == nullptr || std::strcmp(linked_name, "BundledCyclone010") != 0)
        {
            std::cerr << "FAIL: bundled linked_provider mismatch\n";
            return 1;
        }
    }
    else if (linked_name == nullptr || std::strcmp(linked_name, "ExternalCyclone") != 0)
    {
        std::cerr << "FAIL: external linked_provider mismatch\n";
        return 1;
    }

    (void)factory;
    return 0;
}
