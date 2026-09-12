// Copyright (c) 2026 topsun-bot.
// Common Unitree DDS topic names expected by DimOS / G1 / Go2 / R1 code.
// These strings match upstream unitree_sdk2 dds_wrapper defaults.
#pragma once

namespace unitree_hzj
{
namespace topics
{

inline constexpr const char* kLowCmd = "rt/lowcmd";
inline constexpr const char* kLowState = "rt/lowstate";
inline constexpr const char* kArmSdk = "rt/arm_sdk";
inline constexpr const char* kSportModeState = "rt/sportmodestate";
inline constexpr const char* kWirelessController = "rt/wirelesscontroller";
inline constexpr const char* kInspireHandState = "rt/inspire/state";
inline constexpr const char* kDex3LeftHandState = "rt/dex3/left/state";
inline constexpr const char* kDex3RightHandState = "rt/dex3/right/state";

// Request/response channel naming used by ChannelNamer (robot API clients).
inline constexpr const char* kApiPrefix = "rt/api/";
inline constexpr const char* kApiClientSuffix = "/request";
inline constexpr const char* kApiServerSuffix = "/response";

}  // namespace topics
}  // namespace unitree_hzj
