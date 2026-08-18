MKPATH=$(abspath $(lastword $(MAKEFILE_LIST)))

SOURCEDIR=src
TOPDIR=../../../..
INCDIR=-I$(TOPDIR)/include -Ibuild/generated

ifeq ($(OS),Windows_NT)
VULKAN_SDK_PATH=$(subst \,/,$(VULKAN_SDK))
VULKAN_INC=-I$(VULKAN_SDK_PATH)/Include
VULKAN_LIB=-L$(VULKAN_SDK_PATH)/Lib -lvulkan-1
VULKAN_RPATH=
GLSLANG_VALIDATOR=$(VULKAN_SDK_PATH)/Bin/glslangValidator.exe
else
HOST_OS=$(shell uname -s)
ifeq ($(HOST_OS),Darwin)
# LunarG sets VULKAN_SDK in its setup environment. If it is absent, select the
# newest SDK in the standard per-user install location used by the macOS SDK.
VULKAN_SDK ?= $(shell find "$(HOME)/VulkanSDK" -mindepth 2 -maxdepth 2 -type d -name macOS 2>/dev/null | sort -V | tail -1)
VULKAN_SDK_PATH=$(VULKAN_SDK)
VULKAN_INC=-I$(VULKAN_SDK_PATH)/include
VULKAN_LIB=-L$(VULKAN_SDK_PATH)/lib -lvulkan
VULKAN_RPATH=-Wl,-rpath,$(VULKAN_SDK_PATH)/lib
GLSLANG_VALIDATOR=$(VULKAN_SDK_PATH)/bin/glslangValidator
override EXTRACXXFLAGS += -DFI_GFX_METAL
else
VULKAN_SDK_PATH=$(VULKAN_SDK)
VULKAN_INC=$(if $(VULKAN_SDK_PATH),-I$(VULKAN_SDK_PATH)/include)
VULKAN_LIB=$(if $(VULKAN_SDK_PATH),-L$(VULKAN_SDK_PATH)/lib) -lvulkan
VULKAN_RPATH=$(if $(VULKAN_SDK_PATH),-Wl,-rpath,$(VULKAN_SDK_PATH)/lib)
GLSLANG_VALIDATOR=glslangValidator
endif
endif

INCDIR += $(VULKAN_INC)
override EXTRACXXFLAGS += -DFI_GFX_VULKAN
export EXTRACXXFLAGS

LIBS_LINUX=-lfiApp $(VULKAN_LIB) -ldl -lX11
ifeq ($(TOOLCHAIN),llvm-msvc)
LIBS_W32=$(TOPDIR)/lib/libfiApp.a $(VULKAN_LIB) -lgdi32 -luser32
else
LIBS_W32=-lfiApp $(VULKAN_LIB) -lgdi32 -luser32
endif
LIBS_OSX=-lfiApp $(VULKAN_LIB) $(VULKAN_RPATH) -framework Cocoa -framework QuartzCore -framework Metal

CXX_FILES=\
	app.cpp \
	main.cpp

TARGET=vulkanAppTest
CLEAN_TARGET=$(TARGET)
RESOURCES=

DEPENDENCIES=\
	$(TOPDIR)/projects/fiApp

SHADER_DIR=build/generated
VERT_SPV=$(SHADER_DIR)/triangle.vert.spv
FRAG_SPV=$(SHADER_DIR)/triangle.frag.spv
SHADER_HEADERS=$(VERT_SPV).h $(FRAG_SPV).h
CLEAN_EXTRA += $(SHADER_DIR)

include $(TOPDIR)/templates/Makefile_include_application

$(OBJDIR)/app.o: $(SHADER_HEADERS)

$(VERT_SPV): src/triangle.vert
	@command -v "$(GLSLANG_VALIDATOR)" >/dev/null 2>&1 || (echo "Missing glslangValidator at $(GLSLANG_VALIDATOR); install glslang-tools or the LunarG Vulkan SDK."; exit 1)
	@mkdir -p $(SHADER_DIR)
	"$(GLSLANG_VALIDATOR)" -V --target-env vulkan1.0 -S vert -o $@ $<

$(FRAG_SPV): src/triangle.frag
	@command -v "$(GLSLANG_VALIDATOR)" >/dev/null 2>&1 || (echo "Missing glslangValidator at $(GLSLANG_VALIDATOR); install glslang-tools or the LunarG Vulkan SDK."; exit 1)
	@mkdir -p $(SHADER_DIR)
	"$(GLSLANG_VALIDATOR)" -V --target-env vulkan1.0 -S frag -o $@ $<

$(VERT_SPV).h: $(VERT_SPV)
	@command -v xxd >/dev/null 2>&1 || (echo "Missing xxd; install Git for Windows with its standard Unix tools."; exit 1)
	@cd $(SHADER_DIR) && xxd -i triangle.vert.spv > triangle.vert.spv.h

$(FRAG_SPV).h: $(FRAG_SPV)
	@command -v xxd >/dev/null 2>&1 || (echo "Missing xxd; install Git for Windows with its standard Unix tools."; exit 1)
	@cd $(SHADER_DIR) && xxd -i triangle.frag.spv > triangle.frag.spv.h
