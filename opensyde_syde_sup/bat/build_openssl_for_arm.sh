#!/bin/bash
# Script to download and cross-compile OpenSSL for ARM64/AArch64
# This creates ARM-compatible OpenSSL libraries for cross-compilation

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/temp_openssl_build"
INSTALL_DIR="${PROJECT_ROOT}/arm_sysroot"
OPENSSL_VERSION="3.0.13"
OPENSSL_URL="https://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz"

# Detect architecture
ARCH="${1:-aarch64}"

if [ "$ARCH" = "aarch64" ] || [ "$ARCH" = "arm64" ]; then
    CROSS_PREFIX="aarch64-linux-gnu"
    OPENSSL_TARGET="linux-aarch64"
    echo "Building OpenSSL for ARM64/AArch64"
elif [ "$ARCH" = "armv7" ] || [ "$ARCH" = "arm" ]; then
    CROSS_PREFIX="arm-linux-gnueabihf"
    OPENSSL_TARGET="linux-armv4"
    echo "Building OpenSSL for ARMv7 (32-bit)"
else
    echo "Unknown architecture: $ARCH"
    echo "Usage: $0 [aarch64|armv7]"
    exit 1
fi

# Check if cross-compiler is available
if ! command -v ${CROSS_PREFIX}-gcc &> /dev/null; then
    echo "Error: ${CROSS_PREFIX}-gcc not found!"
    echo "Please install the ARM cross-compiler:"
    echo "  brew install ${CROSS_PREFIX}"
    exit 1
fi

echo "==================================================="
echo "OpenSSL ARM Cross-Compilation Setup"
echo "==================================================="
echo "Version:      ${OPENSSL_VERSION}"
echo "Target:       ${OPENSSL_TARGET}"
echo "Toolchain:    ${CROSS_PREFIX}"
echo "Build dir:    ${BUILD_DIR}"
echo "Install dir:  ${INSTALL_DIR}"
echo "==================================================="

# Create directories
mkdir -p "${BUILD_DIR}"
mkdir -p "${INSTALL_DIR}"

cd "${BUILD_DIR}"

# Download OpenSSL if not already present
if [ ! -f "openssl-${OPENSSL_VERSION}.tar.gz" ]; then
    echo "Downloading OpenSSL ${OPENSSL_VERSION}..."
    curl -L -O "${OPENSSL_URL}"
else
    echo "OpenSSL tarball already downloaded"
fi

# Extract
if [ ! -d "openssl-${OPENSSL_VERSION}" ]; then
    echo "Extracting OpenSSL..."
    tar -xzf "openssl-${OPENSSL_VERSION}.tar.gz"
else
    echo "OpenSSL already extracted"
fi

cd "openssl-${OPENSSL_VERSION}"

# Configure for cross-compilation
echo "Configuring OpenSSL for ${OPENSSL_TARGET}..."
./Configure ${OPENSSL_TARGET} \
    --prefix="${INSTALL_DIR}/usr" \
    --openssldir="${INSTALL_DIR}/usr/ssl" \
    --cross-compile-prefix=${CROSS_PREFIX}- \
    no-shared \
    no-tests

# Build
echo "Building OpenSSL (this may take a few minutes)..."
make -j$(sysctl -n hw.ncpu)

# Install
echo "Installing OpenSSL to ${INSTALL_DIR}..."
make install_sw

echo ""
echo "==================================================="
echo "OpenSSL successfully built and installed!"
echo "==================================================="
echo "Headers:   ${INSTALL_DIR}/usr/include"
echo "Libraries: ${INSTALL_DIR}/usr/lib"
echo ""
echo "You can now build SYDEsup with:"
echo "  cd ${PROJECT_ROOT}/bat"
if [ "$ARCH" = "aarch64" ]; then
    echo "  ./build.sh -t rpi64"
else
    echo "  ./build.sh -t rpi"
fi
echo "==================================================="

# Create a marker file
echo "OpenSSL ${OPENSSL_VERSION} for ${ARCH}" > "${INSTALL_DIR}/OPENSSL_VERSION.txt"
