// Open ChannelFactoryInitialize. Participant construction stays in
// libunitree_sdk2.a (channel_factory.cpp.o / dds_factory_model.cpp.o /
// dds_entity.cpp.o). We call those symbols; we do not --wrap them.

#include <stdexcept>
#include <string>

#include <unitree/robot/channel/channel_factory.hpp>
#include <unitree_hzj/dds/provider.hpp>

namespace unitree_hzj
{
namespace dds
{
namespace
{

void require_bundled_0102_if_default()
{
    if (!using_bundled_cyclone010())
    {
        return;
    }
    const char* ver = linked_cyclone_version();
    if (ver == nullptr || std::string(ver) != "0.10.2")
    {
        throw std::runtime_error(
            "BundledCyclone010: cyclone_link.cpp was not compiled against "
            "DDS_VERSION \"0.10.2\". Do not replace thirdparty/ with ros2_hzj "
            "Cyclone 11.0.1.");
    }
    if (std::string(linked_provider_name()) != "BundledCyclone010")
    {
        throw std::runtime_error(
            "BundledCyclone010: compiled provider name mismatch");
    }
}

}  // namespace

void initialize_channel_factory(int32_t domainId, const std::string& networkInterface)
{
    require_bundled_0102_if_default();
    // Closed-source: unitree::robot::ChannelFactory::Init in libunitree_sdk2.a
    unitree::robot::ChannelFactory::Instance()->Init(domainId, networkInterface);
}

void initialize_channel_factory(const std::string& configFileName)
{
    require_bundled_0102_if_default();
    unitree::robot::ChannelFactory::Instance()->Init(configFileName);
}

}  // namespace dds
}  // namespace unitree_hzj

namespace unitree
{
namespace robot
{

void ChannelFactoryInitialize(int32_t domainId, const std::string& networkInterface)
{
    unitree_hzj::dds::initialize_channel_factory(domainId, networkInterface);
}

void ChannelFactoryInitialize(const std::string& configFileName)
{
    unitree_hzj::dds::initialize_channel_factory(configFileName);
}

}  // namespace robot
}  // namespace unitree
