# KoTR Patches

A derived fork of KoTR_Modern_Patch with CMake build system and additional KoTR patches.

## Overview

This is a fork of [KoTR_Modern_Patch](https://github.com/aleko2144/KoTR_Modern_Patch).  
This repository is maintained as a derived project that builds on the original code rather than a drop-in replacement or a completely standalone project.

## Building

### Requirements

- CMake ≥ 3.16
- A C++ compiler with C++17 support
- Git
- **Linux** (cross-compilation to 32-bit Windows):
  ```bash
  sudo install g++-mingw-w64-i686
  ```

### Build Instructions

#### Linux (cross-compile to 32-bit Windows with MinGW)

```bash
# Clone the repository with submodules
git clone --recurse-submodules https://github.com/SilnyiKrasivyiDiadenka/KoTR_Patches.git
cd KoTR_Patches

# Configure and build
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_SYSTEM_PROCESSOR=i686 \
  -DCMAKE_C_COMPILER=/bin/i686-w64-mingw32-gcc \
  -DCMAKE_CXX_COMPILER=/bin/i686-w64-mingw32-g++
cmake --build build
```
**Note:** In Release builds with Clang or GNU compilers (including MinGW), the CMakeLists.txt applies static linking for `libgcc` and `libstdc++`.

#### Windows (Visual Studio)

```bash
# Clone the repository with submodules
git clone --recurse-submodules https://github.com/SilnyiKrasivyiDiadenka/KoTR_Patches.git
```
1. Open Visual Studio.
2. Choose **File -> Open -> CMake...** and select the top-level `CMakeLists.txt` in the cloned repository.
3. Visual Studio will configure the project automatically.
4. Select your desired configuration (e.g. `Release` / `Win32` / 32-bit) and build via **Build -> Build All**.

## License

This project is licensed under the **MIT License**, the same as the original [KoTR_Modern_Patch] project.
See the [LICENSE](LICENSE) file for the full text.

---

**Upstream:** [KoTR_Modern_Patch](https://github.com/aleko2144/KoTR_Modern_Patch)  
**This fork:** Maintained as a derived project with CMake + additional KoTR patches.
