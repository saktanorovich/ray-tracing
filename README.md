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
- **Win**:
  - Git for Windows/x64 Setup (Git-2.52.0-64-bit)
  - Python3 (python-3.14.2-amd64)
  - CMake (cmake-4.2.1-windows-x86_64.msi)
  - VS (https://visualstudio.microsoft.com/vs/community/)

    - Visual Studio 2026 Developer Command Prompt v18.2.0
  - PY
    -  pip install matplotlib
---

## 📚 Required Libraries

- **Standard C++ Library (std)** — core language support
- **Python3 (python)** — to run tests

---

## 🚀 How to Run (win)

1. **Clone the repository**
   ```bash
   git clone https://github.com/saktanorovich/ray-tracing.git
   cd ray-tracing
2. **Build the project**
   ```bash
   rmdir /s /q build bin
   mkdir build
   cd build
   cmake .. -G "NMake Makefiles"
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
   rm -rf build bin && mkdir build && cd build
   cmake ..
   make
   cd ../
3. **Run the application**
   ```bash
   python3 ./tests.py
   python3 ./check.py