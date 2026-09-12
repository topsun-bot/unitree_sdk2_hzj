// Copyright (c) 2026 topsun-bot. HZJ rewrite layer.
#pragma once

#define UNITREE_SDK2_HZJ_VERSION "0.1.0"
#define UNITREE_SDK2_HZJ_VERSION_MAJOR 0
#define UNITREE_SDK2_HZJ_VERSION_MINOR 1
#define UNITREE_SDK2_HZJ_VERSION_PATCH 0

// Snapshot of topsun-bot/unitree_sdk2 used as the original-API source of truth.
#define UNITREE_HZJ_UPSTREAM_REPO "https://github.com/topsun-bot/unitree_sdk2"
#define UNITREE_HZJ_UPSTREAM_COMMIT "9754cd153af3da471b0fe5f3aa535e426fb11db3"

namespace unitree_hzj
{

inline constexpr const char* kVersion = UNITREE_SDK2_HZJ_VERSION;
inline constexpr const char* kUpstreamCommit = UNITREE_HZJ_UPSTREAM_COMMIT;

}  // namespace unitree_hzj
