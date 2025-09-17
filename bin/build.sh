#!/usr/bin/env bash
set -euo pipefail

info() {
  echo "[info] $1"
}
error() {
  echo "[error] $1" >&2;
}

BUILD_TYPE="Debug"
JOBS="$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"
RUN_TESTS=0
CLEAN=0
TARGET=""

usage() {
  cat <<EOF
Usage: $(basename "$0") [options]
  -r, --release Build   Release (default: Debug)
  -d, --debug           Build Debug
  -t, --tests           Run tests after build (ctest)
  -c, --clean           Delete the build dir before configuring
  -T, --target NAME     Build a specific target (e.g., 01_singleton, all_patterns)
  -h, --help
EOF
  exit 0
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    -r|--release) BUILD_TYPE="Release"; shift ;;
    -d|--debug)   BUILD_TYPE="Debug";   shift ;;
    -t|--tests)   RUN_TESTS=1;          shift ;;
    -c|--clean)   CLEAN=1;              shift ;;
    -T|--target)  TARGET="${2:?}";      shift 2 ;;
    -h|--help)    usage ;;
    *) error "Unknown arg: $1"; usage ;;
  esac
done

SCRIPTS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPTS_DIR}/.." && pwd)"

VCPKG_ROOT="${VCPKG_ROOT:-${REPO_ROOT}/vcpkg}"
TOOLCHAIN_FILE="${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"

BUILD_DIR="${REPO_ROOT}/build"
if [[ "${BUILD_TYPE}" == "Debug" ]]; then
  BUILD_DIR="${BUILD_DIR}/debug"
else
  BUILD_DIR="${BUILD_DIR}/release"
fi

ensure_vcpkg() {
  if [[ ! -d "${VCPKG_ROOT}/.git" ]]; then
    info "Cloning vcpkg into ${VCPKG_ROOT}"
    git clone https://github.com/microsoft/vcpkg.git "${VCPKG_ROOT}"
  fi
  if [[ ! -x "${VCPKG_ROOT}/vcpkg" ]]; then
    info "Bootstapping vcpkg"
    bash "${VCPKG_ROOT}/bootstrap-vcpkg.sh"
  fi
}

if [[ ${CLEAN} -eq 1 ]]; then
  info "[clean] Removing ${BUILD_DIR}"
  rm -rf "${BUILD_DIR}"
fi

ensure_vcpkg

cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}" \
  -G Ninja \
  -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
  -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" \
  -DCMAKE_C_COMPILER=/usr/local/opt/llvm/bin/clang \
  -DCMAKE_CXX_COMPILER=/usr/local/opt/llvm/bin/clang++ \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
  -DVCPKG_FEATURE_FLAGS=manifests \
  -DVCPKG_TARGET_TRIPLET=x64-osx \
  -DBUILD_TESTING=ON

if [[ -n "${TARGET}" ]]; then
  info "[build] Target: ${TARGET}"
  cmake --build "${BUILD_DIR}" --target "${TARGET}" -j "${JOBS}"
else
  info "[build] All targets"
  cmake --build "${BUILD_DIR}" -j "${JOBS}"
fi

if [[ ${RUN_TESTS} -eq 1 ]]; then
  info "[test] Running ctest"
  ctest --test-dir "${BUILD_DIR}" --output-on-failure
fi

info "[done] Build type=${BUILD_TYPE}, dir=${BUILD_DIR}"