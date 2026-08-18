MKPATH=$(abspath $(lastword $(MAKEFILE_LIST)))

SOURCEDIR=src
TOPDIR=../../..
INCDIR=-I$(TOPDIR)/include
#LIBDIR=-L$(TOPDIR)/lib

LIBS_LINUX= -lfiApp -ldl -lGL -lX11
ifeq ($(TOOLCHAIN),llvm-msvc)
LIBS_W32= $(TOPDIR)/lib/libfiApp.a -lopengl32 -lgdi32 -luser32
else
LIBS_W32= -lfiApp -lopengl32 -lgdi32 -luser32
endif
LIBS_OSX= -lfiApp -framework Cocoa -framework OpenGL -framework CoreVideo
LIBS_WEB= -lfiApp

ifdef WEB
ifneq ($(findstring FI_GFX_WEBGPU,$(EXTRACXXFLAGS)),)
# Drop any externally-passed WebGL-only linker flags from compile flags.
override EXTRACXXFLAGS := $(filter-out -sMIN_WEBGL_VERSION=% -sMAX_WEBGL_VERSION=% -sFULL_ES3 -sFULL_ES2,$(EXTRACXXFLAGS))
EMSCRIPTEN_GFX_FLAGS += --use-port=emdawnwebgpu
# Template makefiles force -D_GLES2 for WEB; cancel it for WebGPU builds.
CXXFLAGS_WEB += -U_GLES2
override EXTRACXXFLAGS += $(EMSCRIPTEN_GFX_FLAGS)
endif

EMSCRIPTEN_GFX_FLAGS += -sMIN_WEBGL_VERSION=1 -sMAX_WEBGL_VERSION=1 -sFULL_ES3 -sFULL_ES2
LIBS_WEB += $(EMSCRIPTEN_GFX_FLAGS)
endif

### Input Source Files
C_FILES=
ifneq ($(findstring FI_GFX_WEBGPU,$(EXTRACXXFLAGS)),)
CXX_FILES=\
	app_wgpu.cpp \
	main.cpp
else
CXX_FILES=\
	app.cpp \
	main.cpp
endif
OBJC_FILES=
OBJCXX_FILES=
#C_FILES_LINUX
#CXX_FILES_LINUX
#C_FILES_WIN
#CXX_FILES_WIN
#C_FILES_OSX
#CXX_FILES_OSX
#OBJC_FILES_OSX
#OBJCXX_FILES_OSX

### Targets / Output / Resource folders and files
TARGET=appTest
CLEAN_TARGET=$(TARGET)
RESOURCES=

### Specify project dependencies here
DEP_WIN32=
DEP_OSX=
DEP_LINUX=
DEPENDENCIES=\
	$(TOPDIR)/projects/fiApp

### Post Build Steps
POSTBUILD=
POSTBUILDCLEAN=

include $(TOPDIR)/templates/Makefile_include_application
