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

C_FILES_LINUX=\
	glProcs.c
CXX_FILES_LINUX=\
	x11App.cpp

#C_FILES_WIN
CXX_FILES_WIN=\
	win32App.cpp \
	d3d9App.cpp

C_FILES_OSX=\
	glProcs.c
CXX_FILES_OSX=\
	osxApp.cpp \
	cocoaWrapper.cpp
OBJC_FILES_OSX=\
	cocoaGLWindow.m
#OBJCXX_FILES_OSX=\
	cocoaGLWindow.mm

### Targets / Output / Resource folders and files
TARGET=libgfxApp.a
CLEAN_TARGET=$(TARGET)
RESOURCES=

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
