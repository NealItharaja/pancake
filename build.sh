#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${ROOT_DIR}/cmakebuild"

echo "[*] Creating build directory at ${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"

echo "[*] Configuring with CMake"
cmake -S "${ROOT_DIR}/static" -B "${BUILD_DIR}"

START=$(date +%s)

echo "[*] Building Pancake"
cmake --build "${BUILD_DIR}" -j"$(sysctl -n hw.ncpu)"

END=$(date +%s)

echo "[✓] Build completed successfully"
echo "[✓] Total build time (real) = $(( END - START )) seconds"
