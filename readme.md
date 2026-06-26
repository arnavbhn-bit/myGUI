# myGUI: Modern Cross-Platform GUI Library

A raw, lightweight, and modern desktop graphics library built from scratch using pure C, wgpu-native, and GLFW. 

## 🚀 The Problem
Most WGPU-native projects use Rust as the main language and/or winit for window management. I have nothing personal against them but what about someone coming from good old OpenGL (like me) who doesn't want to use Rust/C++ but pure C and GLFW. The biggest advantage is the somewhat cross-platform nature of all these technologies.

---

## 📊 Platform Compatibility Matrix

| Display Server / Platform | Hardware Acceleration Backend | Supported | Status / Notes |
| :--- | :--- | :---: | :--- |
| Linux (Wayland Native) | Vulkan | ✅ | Fully Native, Tearing-Free & Secure. Direct Surface Binding. |
| Windows (Win32) | DirectX 12 / Vulkan | ❌ | *Coming soon!* |
| macOS (Metal) | Metal | ❌ | *Coming soon!* |
| Linux (XWayland Emulation) | Vulkan / OpenGL | ❌ | *Intentionally disabled (Wayland preferred).* |
| Linux (X11 Raw) | Vulkan / Xlib | ❌ | *Intentionally disabled (Wayland preferred).* |

---

## 📦 Prerequisites (Debian 13 / Ubuntu)

Before building, make sure your system has the standard Wayland client development libraries and protocols:
```bash
sudo apt update
sudo apt install cmake xorg-dev libwayland-dev libxkbcommon-dev wayland-protocols libegl1-mesa-dev libdecor-0-dev
```
*Note: This project strictly links against a custom-built static libglfw3.a (alternative as per platform) compiled with native platform flags Eg. (-D GLFW_BUILD_WAYLAND=ON) to make the binaries portable. *

---

## 🛠️ How to Compile & Run

We use a fully dependency-free, explicit static compilation workflow. Run the following command from the repository root:

```bash
gcc src/main.c src/gui.c src/render.c -o app \
    -Iinclude \
    lib/libglfw3.a \
    lib/libwgpu_native.a \
    -lm -ldl -lpthread \
    -lwayland-client -lwayland-cursor -lwayland-egl -lxkbcommon
```
*Note: Plan to move to CMake or [nob.h]https://github.com/tsoding/nob.h/ in the future. *

### Launch the Application:
```
./app
```