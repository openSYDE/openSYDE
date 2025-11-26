# Quick Start: Building SYDEsup for Raspberry Pi

## One-Time Setup

### 1. Build ARM OpenSSL (Required - First Time Only)
```bash
cd bat
./build_openssl_for_arm.sh aarch64
```

This takes about 3-5 minutes and creates ARM-compatible OpenSSL libraries in `arm_sysroot/`.

**You only need to do this once!** The ARM OpenSSL will be reused for all future builds.

## Building

### For 64-bit Raspberry Pi (Pi 3/4/5)
```bash
cd bat
./build.sh -t rpi64
```

Output: `result/raspberrypi64/SYDEsup`

### For 32-bit Raspberry Pi (if you have the toolchain)
```bash
cd bat
./build.sh -t rpi
```

Output: `result/raspberrypi/SYDEsup`

## Deploying

### Copy to Raspberry Pi
```bash
scp result/raspberrypi64/SYDEsup pi@raspberrypi.local:~/
```

### Test on Raspberry Pi
```bash
ssh pi@raspberrypi.local
chmod +x ~/SYDEsup
./SYDEsup -h
./SYDEsup -v
```

## Troubleshooting

### "ARM OpenSSL not found" warning
Run the setup command once:
```bash
cd bat
./build_openssl_for_arm.sh aarch64
```

### "aarch64-linux-gnu-gcc: command not found"
Install the cross-compiler:
```bash
brew install aarch64-linux-gnu
```

### "Exec format error" on Raspberry Pi
Check your Pi's architecture:
```bash
# On Raspberry Pi:
uname -m
```

- If it shows `aarch64`: Use 64-bit build (`./build.sh -t rpi64`)
- If it shows `armv7l`: You need 32-bit build and toolchain

## File Structure

After setup:
```
opensyde_syde_sup/
├── arm_sysroot/              # ARM OpenSSL (created by setup script)
│   └── usr/
│       ├── include/openssl/  # OpenSSL headers
│       └── lib/              # OpenSSL libraries (libcrypto.a, libssl.a)
├── bat/
│   ├── build.sh             # Main build script
│   └── build_openssl_for_arm.sh  # OpenSSL setup script
├── pjt/
│   ├── toolchain_raspberrypi64.cmake  # 64-bit ARM toolchain
│   └── toolchain_raspberrypi.cmake     # 32-bit ARM toolchain
└── result/
    └── raspberrypi64/       # Build output
        ├── SYDEsup          # Executable
        └── sydesup_lib/     # Shared library
```

## Full Documentation

- **Comprehensive Guide:** [doc/target_integration/raspberry_pi_cross_compile.md](doc/target_integration/raspberry_pi_cross_compile.md)
- **Full README:** [README_RASPBERRY_PI.md](README_RASPBERRY_PI.md)
