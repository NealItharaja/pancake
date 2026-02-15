#!/usr/bin/env bash
set -euo pipefail

# Absolute path to project root (where build.sh lives)
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${ROOT_DIR}/cmakebuild"

echo "[*] Creating build directory at ${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"

echo "[*] Configuring with CMake"
cmake -S "${ROOT_DIR}/static" -B "${BUILD_DIR}"

START=$(date +%s)

echo "[*] Building Pancake"
# Use nproc on Linux, sysctl on macOS
if command -v nproc >/dev/null 2>&1; then
    JOBS=$(nproc)
else
    JOBS=$(sysctl -n hw.ncpu)
fi

cmake --build "${BUILD_DIR}" -j"${JOBS}"

END=$(date +%s)

echo "[✓] Build completed successfully"
echo "[✓] Total build time (real) = $(( END - START )) seconds"
