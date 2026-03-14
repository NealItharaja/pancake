#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

BUILD_DIR="${ROOT_DIR}/cmakebuild"

echo "[*] Creating build directory at ${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

echo "[*] Configuring with CMake"
cmake "$@" "${ROOT_DIR}"

START=$(date +%s)

echo "[*] Building Pancake"
if command -v nproc > /dev/null 2>&1; then
    make -j"$(nproc)"
elif command -v sysctl > /dev/null 2>&1; then
    make -j"$(sysctl -n hw.ncpu)"
else
    make -j4
fi

END=$(date +%s)

echo "[✓] Build completed successfully"
echo "[✓] Total build time (real) = $(( END - START )) seconds"
