# thirdparty CycloneDDS (copied, not a submodule)

This directory is a **copy** of `thirdparty/` from
[topsun-bot/unitree_sdk2](https://github.com/topsun-bot/unitree_sdk2) at
`9754cd153af3da471b0fe5f3aa535e426fb11db3`.

| Item | Value |
|------|--------|
| Product | Eclipse CycloneDDS + C++ binding |
| Version | **0.10.2** (`include/dds/version.h` `DDS_VERSION "0.10.2"`) |
| Libraries | `lib/x86_64/libddsc.so`, `libddscxx.so` and aarch64 twins |
| Default HZJ provider | `BundledCyclone010` |

Do not replace these files with a newer Cyclone from ros2_hzj (or anywhere
else). That companion workspace vendors a different major/minor and is not
a drop-in `.so`. See `docs/DDS.md`.

`scripts/assert_bundled_cyclone_0102.sh` fails if `version.h` is not `0.10.2`.

Licenses: `../licenses/eclipse-cyclonedds/`.
