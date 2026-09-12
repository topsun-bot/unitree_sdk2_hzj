# DDS path (unitree_sdk2_hzj)

This document is the first-cut map of how DDS traffic moves through this
tree, what to validate after edits, and the ABI trap between bundled
Cyclone **0.10.2** and ros2_hzj Cyclone **11.0.1**.

Wire interop with a physical Unitree robot is **UNPROVEN** for this HZJ
tree. Do not treat a green CI structure job as robot-ready.

## Relationship

| Tree | Role |
|------|------|
| [topsun-bot/unitree_sdk2](https://github.com/topsun-bot/unitree_sdk2) `@ 9754cd15` | Tracking fork of `unitreerobotics/unitree_sdk2`. Source of the copied headers, `libunitree_sdk2.a`, and thirdparty Cyclone 0.10.2. |
| **this repo** | Independent HZJ product. Rewritten provider / CMake / docs. Original `unitree::robot::ChannelFactory` APIs stay on the default provider. |
| [topsun-bot/ros2_hzj](https://github.com/topsun-bot/ros2_hzj) `v0.1.0 @ 218903b` | Companion middleware workspace. Vendors Cyclone **11.0.1**. **Not** a drop-in `.so`. `fastdds.xml` / SCOREBOARD live there — do not edit them here. |
| topsun_dimos | Future consumer. No PR into DimOS from this work. |

No Agnocast. No zenoh.

## Flow: ChannelFactoryInitialize → provider → closed Init → Cyclone

```
application
  ChannelFactoryInitialize(domainId, nic)     // compiled in libunitree_hzj_dds
        │
        ▼
  unitree_hzj::dds::initialize_channel_factory
        │  src/unitree_hzj/channel.cpp
        │  consults compiled-in provider (cyclone_link.cpp + dds/version.h)
        │  bundled: require linked DDS_VERSION == 0.10.2
        ▼
  ChannelFactory::Init(...)                   // CLOSED — libunitree_sdk2.a
        │  channel_factory.cpp.o
        │  no --wrap, no fake hook
        ▼
  DdsFactoryModel::Init / DdsParticipant      // CLOSED — same archive
        │  dds_factory_model.cpp.o / dds_entity.cpp.o
        │  CreateTopicChannel / SetWriter / SetReader are header templates
        ▼
  CycloneDDS C++ API  (org::eclipse::cyclonedds)
        │
        ├─ BundledCyclone010 (DEFAULT compile + link)
        │    thirdparty/include + thirdparty/lib/<arch>/libddsc.so
        │    cyclone_link.cpp compiled against those headers
        │    version.h DDS_VERSION "0.10.2"
        │
        └─ ExternalCyclone (OPT-IN compile + link)
             UNITREE_HZJ_DDS_ROOT/{include,lib}
             cyclone_link.cpp compiled against that prefix
             ABI vs 0.10.2 UNPROVEN; robot wire UNPROVEN
```

`ChannelFactory::Instance()->Init(...)` still exists and still jumps
straight into the archive. Prefer `ChannelFactoryInitialize` so the
compiled provider runs first.

## What cannot be rewritten (closed static lib)

`nm -C lib/x86_64/libunitree_sdk2.a` shows these **defined** in the archive:

| Object | Symbols |
|--------|---------|
| `channel_factory.cpp.o` | `ChannelFactory::Init` (all overloads), `Release`, ctor |
| `dds_factory_model.cpp.o` | `DdsFactoryModel::Init` / ctor / dtor |
| `dds_entity.cpp.o` | `DdsParticipant`, `DdsPublisher`, `DdsSubscriber` ctors |

There is no Unitree source for those objects in this repo. Replacing them
would mean either omitting the `.a` (breaks every robot client) or
`--wrap` / object extraction (a fake hook). HZJ does **not** do that.

Open and rewritten instead:

- `ChannelFactoryInitialize` — compiled, goes through `provider.hpp`
- `src/unitree_hzj/cyclone_link.cpp` — real compile-time Cyclone include/link
- `dds_wrapper` robot helpers — default topics are `unitree_hzj::topics::*`

`dds_wrapper` (`include/unitree/dds_wrapper/`) sits on top of
`ChannelPublisher` / `ChannelSubscriber`. Robot helpers default to the
same topic strings DimOS/G1 already use:

| Constant (`unitree_hzj::topics`) | Wire name |
|----------------------------------|-----------|
| `kLowCmd` | `rt/lowcmd` |
| `kLowState` | `rt/lowstate` |
| `kArmSdk` | `rt/arm_sdk` |
| `kSportModeState` | `rt/sportmodestate` |
| `kWirelessController` | `rt/wirelesscontroller` |

API client/server channels use `rt/api/<name>/request` and
`rt/api/<name>/response` (`ChannelNamer`).

`DdsParticipant`, `DdsFactoryModel::Init`, and `ChannelFactory::Init` are
**not** header-only. They live in the copied `lib/<arch>/libunitree_sdk2.a`,
which was built against Cyclone 0.10.2 headers. That is why the default
provider must keep linking the copied 0.10.2 `.so` files.

## Providers

CMake:

```bash
# default — must stay 0.10.2
cmake -B build -DUNITREE_HZJ_DDS_PROVIDER=bundled

# opt-in — do not claim this talks to a Unitree robot
cmake -B build \
  -DUNITREE_HZJ_DDS_PROVIDER=external \
  -DUNITREE_HZJ_DDS_ROOT=/path/to/cyclone/prefix
```

| Provider | CMake | Compile define | Cyclone |
|----------|-------|----------------|---------|
| `BundledCyclone010` | `bundled` (default) | `UNITREE_HZJ_DDS_PROVIDER_BUNDLED` | copied `thirdparty/` 0.10.2 |
| `ExternalCyclone` | `external` | `UNITREE_HZJ_DDS_PROVIDER_EXTERNAL` | `UNITREE_HZJ_DDS_ROOT` |

Headers: `include/unitree_hzj/dds/provider.hpp`,
`bundled_cyclone010.hpp`, `external_cyclone.hpp`.

On the bundled path, `bundled_cyclone010.hpp` `static_assert`s
`DDS_VERSION_* == 0.10.2`. CMake also reads `version.h` and fatals if the
string is not `"0.10.2"`.

## Validation points

After any edit that touches DDS, CMake, or `thirdparty/`:

1. **Version assert (required, default path)**
   ```bash
   bash scripts/assert_bundled_cyclone_0102.sh
   ```
   Fails if `thirdparty/include/dds/version.h` is not `DDS_VERSION "0.10.2"`
   or if `libddsc.so` / `libddscxx.so` are missing for x86_64 / aarch64.

2. **Structure / 11.0.1 guard (required)**
   ```bash
   bash scripts/ci_structure_check.sh
   ```
   Fails if `thirdparty/` contains `DDS_VERSION "11.0.1"`,
   `CycloneDDS 11.0.1`, or `cyclonedds-11.0.1` markers.

3. **Configure default provider**
   ```bash
   cmake -B build -DBUILD_EXAMPLES=OFF -DBUILD_HZJ_SMOKE=ON
   ```
   Must print `HZJ DDS provider: BundledCyclone010 (CycloneDDS 0.10.2)`.

4. **Link smoke (when the VM/runner can load the prebuilt `.a` / `.so`)**
   ```bash
   cmake --build build --target hzj_channel_init_smoke
   ./build/bin/hzj_channel_init_smoke
   ```
   Default-path binaries get a `DT_RPATH` to `thirdparty/lib/<arch>` so
   `libddscxx.so.0` can load sibling `libddsc.so.0` (the prebuilt C++ `.so`
   has `RUNPATH $ORIGIN/../lib`, which is the *install* layout, not the
   copied `lib/<arch>` layout).
   Expects `provider BundledCyclone010`, `linked_cyclone 0.10.2`,
   `init_impl closed_static_lib`, `drop_in_for_ros2_hzj false`.
   Does **not** call `Init()` (no live domain).
   Prove the compile switch (same 0.10.2 `.so`, different define):
   ```bash
   cmake -B build-ext -DUNITREE_HZJ_DDS_PROVIDER=external \
     -DUNITREE_HZJ_DDS_ROOT="$PWD/thirdparty"
   cmake --build build-ext --target hzj_channel_init_smoke
   ./build-ext/bin/hzj_channel_init_smoke   # provider ExternalCyclone
   ```

5. **Original example (optional)**
   ```bash
   cmake -B build -DBUILD_EXAMPLES=ON
   cmake --build build --target test_publisher
   ```

6. **External provider configure only (optional)**
   Point `UNITREE_HZJ_DDS_ROOT` at a real Cyclone prefix. Expect a CMake
   **warning** that the path is opt-in and robot interop is unproven.
   Do not copy that prefix into `thirdparty/`.

7. **Physical robot (not in CI)** — UNPROVEN. If you run this later, record
   NIC, domain id `0`, topics `rt/lowcmd` / `rt/lowstate`, and which
   provider was linked. Do not declare success from simulation alone.

## Traps

### ABI 0.10.2 vs 11.0.1

- Bundled `libddsc.so` / `libddscxx.so` are Cyclone **0.10.2** (see
  `thirdparty/include/dds/version.h` and strings inside the `.so`:
  `0.10.2__noshm/cyclonedds-0.10.2`).
- ros2_hzj vendors Cyclone **11.0.1**. Soname, headers, and CDR / type
  object layouts are a different generation.
- `libunitree_sdk2.a` was compiled against 0.10.2 `dds/dds.hpp`. Linking
  that archive to 11.x `libddscxx.so` is **ABI-unsafe**.
- Therefore:
  - Default path **must** keep the copied 0.10.2 files.
  - Do **not** copy ros2_hzj `vendor/CycloneDDS` into `thirdparty/`.
  - Do **not** claim drop-in replacement works.
  - `ExternalCyclone` exists so a later experiment can point at another
    prefix without overwriting the default.

### Other

- `fastdds.xml` and SCOREBOARD are ros2_hzj concerns. Leave them there.
- Topic strings are part of the public contract. Changing `rt/lowcmd` or
  `rt/lowstate` breaks DimOS/G1.
- `ChannelFactory::Init` / `Release` are in the static library. Rewriting
  only the headers does not replace that implementation.
- Iceoryx / shm: the bundled 0.10.2 build is labeled `0.10.2__noshm` in
  the `.so` strings. Do not assume shared-memory transport.

## Tests to run after edits

| Edit | Run |
|------|-----|
| `thirdparty/**` | `assert_bundled_cyclone_0102.sh` + `ci_structure_check.sh` (must fail if 11.0.1 landed) |
| `cmake/**`, root `CMakeLists.txt` | default `cmake -B build` + external-provider configure against a dummy/missing root (must fatal) |
| `include/unitree/robot/channel/**` | smoke link; grep DimOS call sites still compile against `ChannelFactoryInitialize` / `ChannelFactory::Init` |
| `include/unitree/common/dds/**` or `dds_wrapper/**` | smoke + helloworld if examples enabled |
| `include/unitree_hzj/**` | smoke; confirm `using_bundled_cyclone010()` is true without `UNITREE_HZJ_DDS_PROVIDER_EXTERNAL` |
| docs only | no binary test; keep this file's topic table in sync with `include/unitree_hzj/dds/topics.hpp` |

CI: `.github/workflows/ci.yml` `structure` job runs the two scripts.
A `configure-default` job tries CMake on `ubuntu-latest`; if the prebuilt
20.04 `.a`/`.so` cannot link on that image, the structure job is still
the merge gate.
