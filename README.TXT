fiApp

A framework for setting up an simple application window for various operating systems with virtually no dependencies.

Features:
- keyboard + mouse support
- window creation, undecorated at specified size and location
- intended for graphics rendering contexts
- OpenGL (up to 3.2 core) and Direct3D 9 (un-maintained) window + render contexts
- An example is provided for how to use same code as an iOS app

Supported platforms:
- Windows (Win32 API via MinGW-w64 or Visual Studio LLVM/Windows SDK)
- MacOS (GNU make will generate an unsigned .app package)
- Linux (GNU make will generate executible)
- iOS & Android (WIP for providing examples of integrating codebase into respective mobile app projects)

Windows setup
-------------

fiApp uses the shared Makefile templates and Windows launcher from the parent fi
repository. Keep this repository at projects/fiApp inside that checkout.

Install Git for Windows, Visual Studio 2022 C++/Clang tools, a Windows SDK, and
GNU Make 4.x. GNU Make can be installed from PowerShell or Command Prompt with:

  winget install --id ezwinports.make --exact

Close old terminals after installation. From the parent fi repository root run:

  startdev-windows.bat

In the Git Bash window opened by that script:

  make --version
  cd projects/fiApp/test
  make deepclean
  make

The launcher selects TOOLCHAIN=llvm-msvc. MinGW-w64 remains available by passing
TOOLCHAIN=mingw when its gcc, g++, and ar commands are already on PATH.

The default Windows SDK API baseline is Windows 7 (0x0601). Override it per build
when newer APIs are required, for example:

  make WINDOWS_MIN_VERSION=0x0A00

The Win32 window implementation and in-repository OpenGL procedure loader build
with the Visual Studio LLVM toolchain; no external GLEW package is required. The
test produces test/appTest.exe and displays the OpenGL triangle sample.

Windows Vulkan support
----------------------

Install the LunarG Vulkan SDK in addition to the Visual Studio C++ tools. Open a
new terminal after installation and verify that VULKAN_SDK names the installed
SDK directory and that its Include/vulkan/vulkan.h and Lib/vulkan-1.lib files
exist.

VulkanApp is compiled only when FI_GFX_VULKAN is enabled, so normal fiApp and
OpenGL builds do not require Vulkan. From the Git Bash development shell, a
Vulkan-enabled build can pass the backend define through the shared flags:

  make deepclean
  make EXTRACXXFLAGS=-DFI_GFX_VULKAN

On Windows, VulkanApp creates a VK_KHR_win32_surface from fiApp's HWND and
HINSTANCE, selects a presentation-capable queue, and manages the window
swapchain. The same class continues to use a Metal surface through MoltenVK on
macOS. The Windows Vulkan build remains compatible with the existing Windows 7
compile baseline because newer DPI APIs are resolved dynamically; actual Vulkan
runtime support depends on the installed GPU driver and Vulkan loader.

A standalone Vulkan triangle sample is in test/vulkan. It compiles its GLSL
shaders with glslangValidator from the Vulkan SDK and builds fiApp with the
Vulkan backend automatically:

  cd projects/fiApp/test/vulkan
  make deepclean
  make
  ./vulkanAppTest.exe

Ubuntu Vulkan support
---------------------

Ubuntu's packages provide the Vulkan headers and loader, diagnostic tools, and
the shader compiler used by the fiApp and fiGfx samples. Install them with:

  sudo apt update
  sudo apt install libvulkan-dev vulkan-tools glslang-tools xxd

The Vulkan runtime also needs a GPU-specific driver. Ubuntu's Intel and AMD
Mesa drivers can be installed with:

  sudo apt install mesa-vulkan-drivers

For NVIDIA hardware, use Ubuntu's recommended proprietary NVIDIA driver instead.
Verify that the loader can see the GPU before building:

  vulkaninfo --summary

Ubuntu's system packages install headers and libraries in the standard system
paths, so VULKAN_SDK does not need to be set. Build and run the standalone
triangle sample with:

  cd projects/fiApp/test/vulkan
  make deepclean
  make
  ./vulkanAppTest

The fiGfx scene test can be built and run with the Vulkan backend from
projects/fiGfx/test/scene:

  ./build_run.sh vulkan

X11 high-DPI support
--------------------

On Linux, fiApp reads the X11 Xft.dpi resource; 96 DPI is a 1x content scale.
Following the standard X11 model, window and drawable dimensions remain the
exact pixel dimensions requested by the application. The content scale is for
sizing UI rather than multiplying the Vulkan swapchain or OpenGL framebuffer.

If the desktop does not publish Xft.dpi, or to test a specific fractional
scale, set FI_X11_SCALE before launching the application. For example:

  FI_X11_SCALE=2 ./gfxSceneTest

An unset override and missing Xft.dpi retain the existing 1x behavior.

The same Makefile is used on macOS. It adds FI_GFX_METAL for fiApp's
CAMetalLayer window, links MoltenVK through the SDK's libvulkan compatibility
library, embeds the SDK library path in the application, and links Cocoa, Metal,
and QuartzCore. VULKAN_SDK is honored when set; otherwise the newest SDK under
~/VulkanSDK/*/macOS is selected automatically. On macOS run:

  cd projects/fiApp/test/vulkan
  make deepclean
  make
