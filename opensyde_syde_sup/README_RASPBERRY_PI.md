# SYDEsup - Raspberry Pi Cross-Compilation Setup

## Quick Start

Your project is now configured for cross-compiling to Raspberry Pi from macOS!

### Build Commands

```bash
cd bat

# For 64-bit Raspberry Pi (Pi 3/4/5 with 64-bit OS) - RECOMMENDED
./build.sh -t rpi64

# For 32-bit Raspberry Pi (if you have the 32-bit toolchain)
./build.sh -t rpi

# For Ubuntu/Linux AMD64 (original)
./build.sh -t ubuntu
# or simply:
./build.sh
```

## Current Status

✅ **Configured:** Cross-compilation toolchain files created
✅ **Detected:** aarch64-linux-gnu-gcc toolchain (64-bit ARM) installed via Homebrew
✅ **Updated:** Build script supports multiple target platforms
✅ **Tested:** CMake configuration successful

## What Was Added

### New Files:
1. **[pjt/toolchain_raspberrypi64.cmake](pjt/toolchain_raspberrypi64.cmake)** - 64-bit ARM toolchain (works with your current setup)
2. **[pjt/toolchain_raspberrypi.cmake](pjt/toolchain_raspberrypi.cmake)** - 32-bit ARM toolchain (requires additional tools)
3. **[doc/target_integration/raspberry_pi_cross_compile.md](doc/target_integration/raspberry_pi_cross_compile.md)** - Comprehensive cross-compilation guide

### Modified Files:
1. **[bat/build.sh](bat/build.sh)** - Added `-t` flag for target platform selection

## Your Toolchain

You have the **aarch64-linux-gnu** (64-bit ARM) toolchain installed:
- Compiler: `/opt/homebrew/bin/aarch64-linux-gnu-gcc`
- Version: GCC 13.3.0

This is perfect for:
- Raspberry Pi 3 (64-bit OS)
- Raspberry Pi 4 (64-bit OS)
- Raspberry Pi 5 (64-bit OS)

## Important Note: OpenSSL Dependency

⚠️ **Potential Issue Detected:**

During configuration, CMake found OpenSSL from Homebrew (`/opt/homebrew/Cellar/openssl@3`), which is compiled for macOS, not Linux ARM. This may cause one of two issues:

### Issue 1: Build Failure
If the build fails with linking errors related to OpenSSL, you have two options:

#### Option A: Install ARM OpenSSL Libraries (Recommended for Production)
You'll need to cross-compile OpenSSL or obtain pre-built ARM versions:

```bash
# Download and cross-compile OpenSSL for ARM64
wget https://www.openssl.org/source/openssl-3.0.0.tar.gz
tar -xzf openssl-3.0.0.tar.gz
cd openssl-3.0.0

# Configure for ARM64 Linux
./Configure linux-aarch64 \
    --prefix=$HOME/arm-sysroot/usr \
    --cross-compile-prefix=aarch64-linux-gnu-

make
make install
```

Then update the toolchain file to use this OpenSSL.

#### Option B: Build Without OpenSSL Security Features
If you don't need secure update features, you could potentially modify the code to work without OpenSSL (requires code changes).

### Issue 2: Runtime Failure
If the build succeeds but crashes on the Raspberry Pi, ensure OpenSSL is installed on the target:

```bash
# On your Raspberry Pi
sudo apt-get update
sudo apt-get install libssl3 libcrypto++8
```

## Testing Your Build

### 1. Attempt to Build
```bash
cd bat
./build.sh -t rpi64
```

### 2. Check the Output
If successful, the binary will be in:
```
result/raspberrypi64/SYDEsup
```

### 3. Verify the Binary
```bash
file result/raspberrypi64/SYDEsup
```

Should show: `ELF 64-bit LSB executable, ARM aarch64, ...`

### 4. Deploy to Raspberry Pi
```bash
# Copy to your Pi
scp result/raspberrypi64/SYDEsup pi@your-pi-address:~/

# SSH to Pi and test
ssh pi@your-pi-address
chmod +x ~/SYDEsup
./SYDEsup -h
```

## Troubleshooting

### Build fails with "cannot find -lssl" or "-lcrypto"
See "OpenSSL Dependency" section above.

### "Exec format error" on Raspberry Pi
- Verify your Pi is running 64-bit OS: `uname -m` should return `aarch64`
- If it returns `armv7l`, you're on 32-bit and need the 32-bit toolchain

### Want to compile for 32-bit Raspberry Pi?
Install the 32-bit toolchain:
```bash
brew tap messense/macos-cross-toolchains
brew install armv7-unknown-linux-gnueabihf

# Or use official ARM toolchains
# See doc/target_integration/raspberry_pi_cross_compile.md for details
```

Then use:
```bash
./build.sh -t rpi
```

## Architecture Matrix

| Raspberry Pi Model | OS Arch | Toolchain Flag | Toolchain Prefix | CMake File |
|-------------------|---------|----------------|------------------|------------|
| Pi Zero, Pi 1 | 32-bit | `-t rpi` | arm-linux-gnueabihf | toolchain_raspberrypi.cmake (ARMv6) |
| Pi 2, 3, 4, 5 | 32-bit | `-t rpi` | arm-linux-gnueabihf | toolchain_raspberrypi.cmake (ARMv7) |
| Pi 3, 4, 5 | **64-bit** | **`-t rpi64`** | **aarch64-linux-gnu** | **toolchain_raspberrypi64.cmake** |

**Your current setup supports the 64-bit option (highlighted in bold).**

## Next Steps

1. **Try building:** Run `./build.sh -t rpi64` and see if it completes
2. **If it fails on OpenSSL:** Follow Option A or B in the OpenSSL section
3. **Test on real hardware:** Deploy to your Raspberry Pi and verify functionality
4. **Optional:** Install 32-bit toolchain if you need 32-bit support

## Additional Documentation

- **Full cross-compilation guide:** [doc/target_integration/raspberry_pi_cross_compile.md](doc/target_integration/raspberry_pi_cross_compile.md)
- **Original verification guide:** [doc/target_integration/target_verify.txt](doc/target_integration/target_verify.txt)

## Support

For issues specific to:
- **Cross-compilation setup:** Check the comprehensive guide in `doc/target_integration/`
- **SYDEsup functionality:** Refer to original documentation
- **Toolchain installation:** See ARM GNU Toolchain documentation

---

**Created:** 2025-11-26
**Toolchain Version:** GCC 13.3.0 (aarch64-linux-gnu)
**macOS Version:** Darwin 24.6.0
