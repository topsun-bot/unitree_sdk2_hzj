#!/usr/bin/env bash
# Fail if the default (bundled) CycloneDDS is not exactly 0.10.2.
# Used by CI and as a local guard after thirdparty edits.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VER_H="${ROOT}/thirdparty/include/dds/version.h"

if [[ ! -f "${VER_H}" ]]; then
  echo "FAIL: missing ${VER_H}" >&2
  echo "Default provider BundledCyclone010 requires copied CycloneDDS 0.10.2 headers." >&2
  exit 1
fi

if ! grep -qE '^#define DDS_VERSION "0\.10\.2"$' "${VER_H}"; then
  echo "FAIL: ${VER_H} is not DDS_VERSION \"0.10.2\"" >&2
  echo "----- version.h -----" >&2
  grep -E 'DDS_VERSION' "${VER_H}" >&2 || true
  echo "Do not replace thirdparty/ with ros2_hzj vendor/CycloneDDS 11.0.1." >&2
  exit 1
fi

for tok in DDS_VERSION_MAJOR DDS_VERSION_MINOR DDS_VERSION_PATCH; do
  case "${tok}" in
    DDS_VERSION_MAJOR) expect=0 ;;
    DDS_VERSION_MINOR) expect=10 ;;
    DDS_VERSION_PATCH) expect=2 ;;
  esac
  if ! grep -qE "^#define ${tok} ${expect}$" "${VER_H}"; then
    echo "FAIL: ${VER_H} ${tok} is not ${expect}" >&2
    exit 1
  fi
done

for arch in x86_64 aarch64; do
  for lib in libddsc.so libddscxx.so; do
    path="${ROOT}/thirdparty/lib/${arch}/${lib}"
    if [[ ! -f "${path}" ]]; then
      echo "FAIL: missing bundled library ${path}" >&2
      exit 1
    fi
  done
done

echo "OK: bundled CycloneDDS is 0.10.2 (thirdparty/include/dds/version.h + libddsc/libddscxx)"
