# unitree_sdk2_hzj

HZJ rewrite of the Unitree SDK2 DDS path. Independent topsun-bot product.

This is **not** a PR into [topsun_dimos](https://github.com/dimensionalOS/dimos)
and it does **not** overwrite [topsun-bot/unitree_sdk2](https://github.com/topsun-bot/unitree_sdk2).

**Wire interop with a physical Unitree robot is UNPROVEN.**

## News

- **0.1.0 first tree** — real source, not a seed README. Default DDS provider
  is still the copied CycloneDDS **0.10.2** (`thirdparty/`,
  `DDS_VERSION "0.10.2"`, `libddsc.so` / `libddscxx.so`).
- **Compiled provider** — `ChannelFactoryInitialize` is implemented in
  `src/unitree_hzj/` and goes through `unitree_hzj::dds` before calling
  closed `ChannelFactory::Init` in `libunitree_sdk2.a`. That Init is not
  wrapped. `UNITREE_HZJ_DDS_PROVIDER=external` is a real compile/link
  switch (`cyclone_link.cpp` + selected include/lib), not headers-only.
- **Provider switch** — `UNITREE_HZJ_DDS_PROVIDER=bundled|external`. External
  is opt-in via `UNITREE_HZJ_DDS_ROOT`. It is **not** a drop-in for
  ros2_hzj's Cyclone 11.0.1.
- **Original APIs stay** — `ChannelFactory::Init`, `ChannelFactoryInitialize`,
  `rt/lowcmd`, `rt/lowstate`, `rt/arm_sdk`, and the rest of the mirrored
  `include/unitree` surface remain on the default provider.
- **No fastdds.xml / SCOREBOARD / Agnocast / zenoh** here. Those belong in
  ros2_hzj or not at all.

## How this repo relates to the others

```
unitree_sdk2 (tracking fork)          ros2_hzj (middleware workspace)
topsun-bot/unitree_sdk2 @ 9754cd15    v0.1.0 @ 218903b
  Cyclone 0.10.2 bundled                vendors Cyclone 11.0.1
  keep for upstream tracking            NOT a drop-in .so
           \                          /
            \                        /
             unitree_sdk2_hzj (this repo)
               default: copy of 0.10.2
               rewrite: HZJ provider + CMake + docs
                        |
                        v
             topsun_dimos  — consume later, no PR now
```

| Repo | Role |
|------|------|
| [topsun-bot/unitree_sdk2](https://github.com/topsun-bot/unitree_sdk2) | Fork of [unitreerobotics/unitree_sdk2](https://github.com/unitreerobotics/unitree_sdk2) at `9754cd15`. Tracking source. Copied into this tree (not a submodule). |
| [topsun-bot/ros2_hzj](https://github.com/topsun-bot/ros2_hzj) | Companion ROS 2 / middleware workspace. Cyclone **11.0.1** is a different ABI. Do not copy it into `thirdparty/`. |
| topsun_dimos | Future consumer of this SDK. Not modified by this work. |

Provenance: [NOTICE](NOTICE). Licenses: [LICENSE](LICENSE) (Unitree BSD-3) and
[licenses/eclipse-cyclonedds](licenses/eclipse-cyclonedds).

## Default provider (BundledCyclone010)

Same link line as upstream unitree_sdk2:

- Headers: `thirdparty/include` (`dds/version.h` → `"0.10.2"`)
- Libs: `thirdparty/lib/<arch>/libddsc.so`, `libddscxx.so`
- SDK archive: `lib/<arch>/libunitree_sdk2.a` (Init / DdsFactoryModel live here)
- Public C++: `unitree::robot::ChannelFactory` and
  `unitree::robot::ChannelFactoryInitialize`

A version assert **fails** if the default path is not 0.10.2:

```bash
bash scripts/assert_bundled_cyclone_0102.sh
```

## External provider (opt-in, unproven)

```bash
cmake -B build \
  -DUNITREE_HZJ_DDS_PROVIDER=external \
  -DUNITREE_HZJ_DDS_ROOT=/path/to/cyclone/prefix
```

`UNITREE_HZJ_DDS_ROOT` must contain `include/dds/version.h` and
`libddsc.so` + `libddscxx.so` under `lib/`, `lib/<arch>/`, or `lib64/`.

This does **not** make ros2_hzj 11.0.1 a supported replacement. The prebuilt
`libunitree_sdk2.a` was compiled against 0.10.2 headers. Mixing that archive
with Cyclone 11.x is ABI-unsafe. Robot wire interop is UNPROVEN.

## How to build

Needs CMake ≥ 3.5, a C++17 compiler, and (for examples) the usual Ubuntu
dev packages. Prebuilt binaries target Ubuntu 20.04 / gcc 9.4
(`aarch64` and `x86_64`).

```bash
# dependencies (Ubuntu)
sudo apt-get update
sudo apt-get install -y cmake g++ build-essential
```

### Default (bundled 0.10.2)

```bash
cmake -B build
cmake --build build
./build/bin/hzj_channel_init_smoke
# If a consumer binary was not built with this repo's BUILD_RPATH:
#   export LD_LIBRARY_PATH="$PWD/thirdparty/lib/$(uname -m):$LD_LIBRARY_PATH"
```

CMake refuses to configure if `thirdparty/include/dds/version.h` is not
`DDS_VERSION "0.10.2"`.

Original helloworld (optional):

```bash
cmake -B build -DBUILD_EXAMPLES=ON
cmake --build build --target test_publisher
```

Install (same imported-target idea as upstream):

```bash
cmake -B build -DCMAKE_INSTALL_PREFIX=/opt/unitree_sdk2_hzj
cmake --build build --target install
```

Consumers still link `unitree_sdk2` (original name) or `unitree_sdk2_hzj`.

### External (opt-in)

```bash
cmake -B build \
  -DUNITREE_HZJ_DDS_PROVIDER=external \
  -DUNITREE_HZJ_DDS_ROOT=/path/to/cyclone/prefix
```

Expect a CMake warning. Do not copy that prefix into `thirdparty/`.

## Using the APIs DimOS / G1 already call

```cpp
#include <unitree/robot/channel/channel_factory.hpp>
#include <unitree/robot/channel/channel_publisher.hpp>
#include <unitree/idl/hg/LowCmd_.hpp>

unitree::robot::ChannelFactoryInitialize(0, "eth0");
// equivalent: ChannelFactory::Instance()->Init(0, "eth0");

unitree::robot::ChannelPublisher<unitree_hg::msg::dds_::LowCmd_> pub("rt/lowcmd");
pub.InitChannel();
```

HZJ umbrella (provider + topic constants):

```cpp
#include <unitree_hzj/unitree_hzj.hpp>

// unitree_hzj::topics::kLowCmd == "rt/lowcmd"
// unitree_hzj::dds::active_provider().name == "BundledCyclone010" on default
```

## Layout

```
include/unitree/           mirrored upstream (ChannelFactory, dds_wrapper, IDL)
include/unitree_hzj/       rewritten DDS integration (providers, topics)
src/unitree_hzj/           compiled provider + ChannelFactoryInitialize
thirdparty/                copied Cyclone 0.10.2 — do not replace with 11.0.1
lib/<arch>/                copied libunitree_sdk2.a (closed Init)
hzj/smoke/                 provider / ChannelFactory link smoke
docs/DDS.md                flow, closed-lib limits, ABI traps
scripts/                   version assert + 11.0.1 structure guard
```

## Docs and CI

- [docs/DDS.md](docs/DDS.md) — ChannelFactory / dds_wrapper flow, validation
  points, 0.10.2 vs 11.0.1 traps, tests to run after edits.
- CI `structure` job: `assert_bundled_cyclone_0102.sh` +
  `ci_structure_check.sh` (fails if `thirdparty/` grows 11.0.1 markers).

## Versioning

This tree is tag-ready for a later `v0.1.0`. No GitHub Release is created
unless someone asks. Keep `main` mergeable.

HZJ version macros: `include/unitree_hzj/version.hpp`.
