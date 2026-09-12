// Copyright (c) 2026 topsun-bot.
// HZJ channel surface. ChannelFactoryInitialize is compiled in
// src/unitree_hzj/channel.cpp and goes through the DDS provider before
// calling closed ChannelFactory::Init in libunitree_sdk2.a.
#pragma once

#include <unitree/robot/channel/channel_factory.hpp>
#include <unitree/robot/channel/channel_publisher.hpp>
#include <unitree/robot/channel/channel_subscriber.hpp>
#include <unitree_hzj/dds/bundled_cyclone010.hpp>
#include <unitree_hzj/dds/external_cyclone.hpp>
#include <unitree_hzj/dds/provider.hpp>
#include <unitree_hzj/dds/topics.hpp>
#include <unitree_hzj/version.hpp>

namespace unitree_hzj
{

using unitree::robot::ChannelFactory;
using unitree::robot::ChannelFactoryInitialize;
using unitree::robot::ChannelPublisher;
using unitree::robot::ChannelSubscriber;

}  // namespace unitree_hzj
