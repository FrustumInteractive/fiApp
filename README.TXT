fiApp

A framework for setting up an simple application window for various operating systems with virtually no dependencies.

Features:
- keyboard + mouse support
- window creation, undecorated at specified size and location
- intended for graphics rendering contexts
- OpenGL (up to 3.2 core) and Direct3D 9 (un-maintained) window + render contexts
- An example is provided for how to use same code as an iOS app

Supported platforms:
- Windows (via Win32 API from MingGW)
- MacOS (GNU make will generate an unsigned .app package)
- Linux (GNU make will generate executible)
- iOS & Android (WIP for providing examples of integrating codebase into respective mobile app projects)
