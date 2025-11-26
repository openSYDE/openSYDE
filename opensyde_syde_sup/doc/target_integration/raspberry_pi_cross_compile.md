# Cross-Compiling SYDEsup for Raspberry Pi from macOS

This guide explains how to cross-compile the SYDEsup tool from macOS (Intel/ARM) to target Raspberry Pi ARM devices.

## Prerequisites

### 1. Install ARM Cross-Compilation Toolchain

You need to install an ARM cross-compiler on your macOS system. There are several options:

#### Option A: Using Homebrew (Recommended for quick start)
```bash
# Install ARM bare-metal toolchain (may need additional setup for Linux targets)
brew install arm-linux-gnueabihf-binutils
```

#### Option B: Using ARM's Official Toolchain (Recommended for production)
1. Download the ARM GNU Toolchain from:
   https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads

2. Look for "AArch32 GNU/Linux target (arm-linux-gnueabihf)" for 32-bit Raspberry Pi
   Or "AArch64 GNU/Linux target (aarch64-linux-gnu)" for 64-bit Raspberry Pi

3. Download the macOS hosted cross-compiler (darwin)

4. Extract the toolchain:
   ```bash
   mkdir -p ~/tools
   tar -xf arm-gnu-toolchain-*-darwin-*-arm-linux-gnueabihf.tar.xz -C ~/tools
   ```

5. Add to your PATH or set ARM_TOOLCHAIN_PATH:
   ```bash
   export ARM_TOOLCHAIN_PATH=~/tools/arm-gnu-toolchain-*-darwin-*-arm-linux-gnueabihf
   export PATH=$ARM_TOOLCHAIN_PATH/bin:$PATH
   ```

   Add this to your `~/.zshrc` or `~/.bash_profile` to make it permanent.

#### Option C: Using Linaro Toolchain
Download from: https://releases.linaro.org/components/toolchain/binaries/

### 2. Verify Toolchain Installation

Check that the cross-compiler is accessible:
```bash
arm-linux-gnueabihf-gcc --version
arm-linux-gnueabihf-g++ --version
```

You should see output indicating the ARM GCC compiler version.

### 3. Install CMake (if not already installed)
```bash
brew install cmake
```

## Raspberry Pi Model Selection

The toolchain is configured for different Raspberry Pi models:

### Default Configuration (Raspberry Pi 2/3/4 - 32-bit)
- Architecture: ARMv7-A
- FPU: NEON with VFPv4
- ABI: hard-float

This is the default configuration in `toolchain_raspberrypi.cmake`.

### Raspberry Pi Zero/1 (ARMv6)
To compile for older models, edit `pjt/toolchain_raspberrypi.cmake` and uncomment the ARMv6 section:
```cmake
# Uncomment these lines:
set(CMAKE_C_FLAGS "-march=armv6 -mfpu=vfp -mfloat-abi=hard -Wno-deprecated-declarations -fPIC" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "-march=armv6 -mfpu=vfp -mfloat-abi=hard -Wno-deprecated-declarations -fPIC" CACHE STRING "" FORCE)
```

### Raspberry Pi 4 (64-bit)
For 64-bit Raspberry Pi OS, edit `pjt/toolchain_raspberrypi.cmake`:
```cmake
# Change the prefix:
set(CROSS_COMPILE_PREFIX "aarch64-linux-gnu")
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_FLAGS "-march=armv8-a -Wno-deprecated-declarations -fPIC" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "-march=armv8-a -Wno-deprecated-declarations -fPIC" CACHE STRING "" FORCE)
```

## Building for Raspberry Pi

### Using the build script (recommended):
```bash
cd bat
./build.sh -t rpi
```

### Manual CMake build:
```bash
# Create build directory
mkdir -p temp
cd temp

# Configure with Raspberry Pi toolchain
cmake ../pjt -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../pjt/toolchain_raspberrypi.cmake

# Build
cmake --build . --target all -- -j4

# Install
cmake --build . --target install
```

## Build Output

The compiled binaries will be placed in:
```
result/raspberrypi/
├── SYDEsup                    # Main executable
└── sydesup_lib/               # Shared library and headers
    ├── libsydesuplib.so
    ├── C_SydeSup.hpp
    ├── C_SydeSupLinux.hpp
    └── ...
```

## Deploying to Raspberry Pi

### 1. Copy the binary to your Raspberry Pi:
```bash
scp result/raspberrypi/SYDEsup pi@raspberrypi.local:~/
```

### 2. Copy shared library (if needed):
```bash
scp result/raspberrypi/sydesup_lib/libsydesuplib.so pi@raspberrypi.local:/usr/local/lib/
```

### 3. SSH into Raspberry Pi and test:
```bash
ssh pi@raspberrypi.local
chmod +x ~/SYDEsup
./SYDEsup -h
```

## Troubleshooting

### Issue: "arm-linux-gnueabihf-gcc: command not found"
**Solution**: The cross-compiler is not in your PATH. Either:
- Install the toolchain (see Prerequisites)
- Set the ARM_TOOLCHAIN_PATH environment variable
- Update your PATH to include the toolchain bin directory

### Issue: "cannot find -lssl" or "cannot find -lcrypto"
**Solution**: You need ARM versions of OpenSSL libraries. You have two options:

1. **Statically link OpenSSL** (already configured in CMakeLists.txt for Linux)
2. **Install OpenSSL on Raspberry Pi** and ensure the cross-compiler can find the ARM libraries:
   ```bash
   # On Raspberry Pi:
   sudo apt-get install libssl-dev
   ```

### Issue: "Exec format error" when running on Raspberry Pi
**Solution**:
- Check that you're using the correct toolchain for your Raspberry Pi model (32-bit vs 64-bit)
- Verify the architecture: `file result/raspberrypi/SYDEsup`
  - Should show "ARM" for 32-bit or "AArch64" for 64-bit
- Ensure your Raspberry Pi OS matches the compiled binary architecture

### Issue: Compilation succeeds but binary crashes on Raspberry Pi
**Solution**:
- Check ARM architecture match (ARMv6 vs ARMv7 vs ARMv8)
- For older Raspberry Pi (Zero/1), use ARMv6 configuration
- Ensure all dependencies are available on the target Raspberry Pi

## Advanced: Using Docker for Cross-Compilation

For a more reproducible build environment, you can use Docker:

```bash
# Create a Dockerfile with the cross-compilation toolchain
docker run -it --rm -v $(pwd):/work \
  gcc:latest bash -c \
  "apt-get update && apt-get install -y gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf cmake && \
   cd /work && cd bat && ./build.sh -t rpi"
```

## Additional Resources

- ARM GNU Toolchain: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain
- Raspberry Pi Documentation: https://www.raspberrypi.org/documentation/
- CMake Cross-Compiling: https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html

## Notes

- The cross-compiled binary is statically linked with OpenSSL to avoid runtime dependency issues
- For CAN bus functionality, ensure your Raspberry Pi has the necessary CAN interface hardware
- The executable requires appropriate permissions for CAN and network interfaces on the target
