MKPATH=$(abspath $(lastword $(MAKEFILE_LIST)))

SOURCEDIR=src
TOPDIR=../../..
INCDIR=-I$(TOPDIR)/include
#LIBDIR=-L$(TOPDIR)/lib

LIBS_LINUX= -lfiApp -ldl -lGL -lX11
LIBS_W32= -ld3d9 -static -lfigfx -lglew32s -lopengl32 -lgdi32
LIBS_OSX= -lfiApp -framework Cocoa -framework OpenGL
LIBS_WEB= -lfiApp

### Input Source Files
C_FILES=
CXX_FILES=\
	app.cpp \
	main.cpp
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
