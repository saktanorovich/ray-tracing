# rt Project

**rt** — a lightweight C++ demo app for ray-traicing.

---

## 📋 Prerequisites

- **Operating System**: macOS (arm64)
- **Compiler**: g++ or clang with C++17 support
- **Build System**: CMake ≥ 3.30
- **Tests** Python3: 3.14.0
- **Tools**:
  - `make` to build the project
  - `git` to clone the repository
  - `glad` to render (https://glad.dav1d.de/)
- **Win**:
  - `git` for Windows/x64 Setup (Git-2.52.0-64-bit)
  - `python3` (python-3.14.2-amd64)
  - `cmake` (cmake-4.2.1-windows-x86_64.msi)
  - `vs` (https://visualstudio.microsoft.com/vs/community/)
  - `vs 2026` Developer Command Prompt v18.2.0
    -  pip install matplotlib
    -  pip install colorama
  - `open` Visual Studio 2026 Developer Command Prompt v18.2.0
  - `vcpkg`
    ```bash
    cd C:\Temp
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    bootstrap-vcpkg.bat
    vcpkg install glfw3:x64-windows
    vcpkg install glm:x64-windows
    cd ../
---

## 🚀 How to Run (win)

1. **Clone the repository**
   ```bash
   git clone https://github.com/saktanorovich/ray-tracing.git
   cd ray-tracing
2. **Build the project**
   ```bash
   if exist build rmdir /s /q build
   if exist bin rmdir /s /q bin
   mkdir build
   cd build
   cmake .. -G "NMake Makefiles" -DCMAKE_TOOLCHAIN_FILE="C:\Temp\vcpkg\scripts\buildsystems\vcpkg.cmake"
   nmake
   cd ../
3. **Run the application**
   ```bash
   python ./tests.py
   python ./check.py

---

## 🚀 How to Run (mac)

1. **Clone the repository**
   ```bash
   git clone https://github.com/saktanorovich/ray-tracing.git
   cd ray-tracing
2. **Build the project**
   ```bash
   rm -rf build bin
   mkdir build && cd build
   cmake ..
   make
   cd ../
3. **Run the application**
   ```bash
   python3 ./tests.py
   python3 ./check.py