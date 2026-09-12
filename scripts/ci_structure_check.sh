#!/usr/bin/env bash
# Structure job: default tree still vendors Cyclone 0.10.2, not ros2_hzj 11.0.1.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TP="${ROOT}/thirdparty"

fail() {
  echo "FAIL: $*" >&2
  exit 1
}

[[ -d "${TP}" ]] || fail "thirdparty/ missing"
[[ -f "${ROOT}/include/unitree/robot/channel/channel_factory.hpp" ]] \
  || fail "ChannelFactory header missing"
[[ -f "${ROOT}/include/unitree/common/dds/dds_factory_model.hpp" ]] \
  || fail "dds_factory_model.hpp missing"
[[ -d "${ROOT}/include/unitree/dds_wrapper" ]] \
  || fail "dds_wrapper missing"
[[ -d "${ROOT}/include/unitree/idl" ]] \
  || fail "IDL headers missing"
[[ -f "${ROOT}/include/unitree_hzj/dds/provider.hpp" ]] \
  || fail "HZJ provider header missing"
[[ -f "${ROOT}/lib/x86_64/libunitree_sdk2.a" ]] \
  || fail "prebuilt libunitree_sdk2.a (x86_64) missing"
[[ -f "${ROOT}/licenses/eclipse-cyclonedds/cyclonedds/LICENSE" ]] \
  || fail "eclipse-cyclonedds license missing"
[[ -f "${ROOT}/LICENSE" ]] || fail "LICENSE missing"

# Must not contain ros2_hzj Cyclone 11.0.1 markers inside thirdparty headers/libs.
# (Do not scan docs/ — they mention 11.0.1 on purpose.)
if grep -R -n -E 'DDS_VERSION[ \t]+"11\.0\.1"|CycloneDDS 11\.0\.1|cyclonedds-11\.0\.1' \
    "${TP}/include" "${TP}/lib" >/tmp/hzj_tp_1101.txt 2>/dev/null; then
  echo "FAIL: thirdparty/ contains Cyclone 11.0.1 markers:" >&2
  cat /tmp/hzj_tp_1101.txt >&2
  echo "Do not copy ros2_hzj vendor/CycloneDDS 11.0.1 into thirdparty/." >&2
  exit 1
fi

# Extra guard: version.h must not say 11.x
if grep -E 'DDS_VERSION[ \t]+"11\.' "${TP}/include/dds/version.h" >/dev/null 2>&1; then
  fail "thirdparty/include/dds/version.h reports a 11.x Cyclone — bundled path must stay 0.10.2"
fi

# Binary .so markers (grep -a; avoid strings|grep + pipefail SIGPIPE).
if [[ -f "${TP}/lib/x86_64/libddsc.so" ]]; then
  if grep -a -F '0.10.2' "${TP}/lib/x86_64/libddsc.so" >/dev/null; then
    echo "OK: libddsc.so (x86_64) contains 0.10.2 path/version strings"
  else
    echo "WARN: libddsc.so (x86_64) has no obvious 0.10.2 string (header still governs)"
  fi
  if grep -a -F '11.0.1' "${TP}/lib/x86_64/libddsc.so" >/dev/null; then
    fail "libddsc.so (x86_64) contains 11.0.1 marker — thirdparty was replaced"
  fi
fi

echo "OK: structure — ChannelFactory/dds_wrapper/IDL present, thirdparty is not Cyclone 11.0.1"
