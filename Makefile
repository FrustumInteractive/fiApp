MKPATH=$(abspath $(lastword $(MAKEFILE_LIST)))

SOURCEDIR=src
TOPDIR=../..
INCDIR=-I$(TOPDIR)/include
LIBDIR=-L$(TOPDIR)/lib

### Input Source Files
C_FILES=
CXX_FILES=\
	application.cpp \
	oglApp.cpp \
	console.cpp \
	fileIO.cpp \
	json.cpp \
	threading.cpp \
	event.cpp

#OBJC_FILES=
#OBJCXX_FILES=

ifneq ($(findstring FI_GFX_VULKAN,$(EXTRACXXFLAGS)),)
CXX_FILES += \
	vulkanApp.cpp

endif

ifneq ($(findstring FI_GFX_METAL,$(EXTRACXXFLAGS)),)
INCDIR += -I../fiGfx/external/metal-cpp
ifeq ($(findstring FI_GFX_VULKAN,$(EXTRACXXFLAGS)),)
CXX_FILES += \
	metalApp.cpp
endif
endif

ifneq ($(findstring FI_GFX_VULKAN,$(EXTRACXXFLAGS)),)
OUTDIR=build/vulkan
else ifneq ($(findstring FI_GFX_METAL,$(EXTRACXXFLAGS)),)
OUTDIR=build/metal
endif

C_FILES_LINUX=\
	gl/glProcs.c
CXX_FILES_LINUX=\
	x11App.cpp

CXX_FILES_WEB=\
	webApp.cpp

C_FILES_WIN=\
	gl/glProcs.c
CXX_FILES_WIN=\
	win32App.cpp \
	d3d9App.cpp

C_FILES_OSX=\
	gl/glProcs.c
CXX_FILES_OSX=\
	osxApp.cpp \
	cocoaWrapper.cpp
OBJC_FILES_OSX=\
	cocoaWindow.m
#OBJCXX_FILES_OSX=\
	cocoaGLWindow.mm

### Targets / Output / Resource folders and files
TARGET=libfiApp.a
CLEAN_TARGET=$(TARGET)
RESOURCES=

ifdef WEB
ifneq ($(findstring FI_GFX_WEBGPU,$(EXTRACXXFLAGS)),)
# Drop any externally-passed WebGL-only linker flags from compile flags.
override EXTRACXXFLAGS := $(filter-out -sMIN_WEBGL_VERSION=% -sMAX_WEBGL_VERSION=% -sFULL_ES3 -sFULL_ES2,$(EXTRACXXFLAGS))
EMSCRIPTEN_GFX_FLAGS += --use-port=emdawnwebgpu
# Template makefiles force -D_GLES2 for WEB; cancel it for WebGPU builds.
CXXFLAGS_WEB += -U_GLES2
# WebGPU builds should not compile GL app wrappers.
CXX_FILES := $(filter-out oglApp.cpp,$(CXX_FILES))
override EXTRACXXFLAGS += $(EMSCRIPTEN_GFX_FLAGS)
endif

EMSCRIPTEN_GFX_FLAGS += -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFULL_ES3 -sFULL_ES2
LIBS_WEB += $(EMSCRIPTEN_GFX_FLAGS)
endif

### Specify project dependencies here
DEP_WIN32=
DEP_OSX=
DEP_LINUX=
DEPENDENCIES=

### Post Build Steps
POSTBUILD:=
POSTBUILDCLEAN:=

### OS specific compile flags
CXXFLAGS_OSX:= -DGL_SILENCE_DEPRECATION

include $(TOPDIR)/templates/Makefile_include_static_lib
