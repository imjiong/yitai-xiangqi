# Makefile for Chinese Chess UCI Client
# Target: Windows (MinGW 32-bit), wxWidgets 3.2.11

CXX := g++
CXXFLAGS := -std=c++17 -Wall -O2

WX_ROOT := C:/wxWidgets-3.2.11
WX_INC := $(WX_ROOT)/include
WX_LIB_INC := $(WX_ROOT)/lib/gcc_dll/mswu
WX_LIB_DIR := $(WX_ROOT)/lib/gcc_dll
MINGW_BIN := C:/MinGW/mingw32/bin

INCLUDES := -I$(WX_INC) -I$(WX_LIB_INC) -Isrc
LDFLAGS := -L$(WX_LIB_DIR)
LIBS := -lwxmsw32u_aui -lwxmsw32u_core -lwxbase32u \
        -lwxpng -lwxjpeg -lwxzlib -lwxregexu -lwxexpat \
        -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lwinspool \
        -lwinmm -lshell32 -lcomctl32 -lole32 -loleaut32 \
        -luuid -lrpcrt4 -ladvapi32 -lws2_32

SRC_DIR := src
BUILD_DIR := build
TARGET := $(BUILD_DIR)/yitai-xiangyi.exe

SOURCES := $(wildcard $(SRC_DIR)/*.cpp) \
           $(wildcard $(SRC_DIR)/core/*.cpp) \
           $(wildcard $(SRC_DIR)/gui/*.cpp) \
           $(wildcard $(SRC_DIR)/engine/*.cpp)

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES)) $(BUILD_DIR)/app.o

WX_DLLS := $(WX_LIB_DIR)/wxbase32u_gcc_custom.dll \
            $(WX_LIB_DIR)/wxmsw32u_core_gcc_custom.dll \
            $(WX_LIB_DIR)/wxmsw32u_aui_gcc_custom.dll

MINGW_DLLS := $(MINGW_BIN)/libstdc++-6.dll \
              $(MINGW_BIN)/libgcc_s_dw2-1.dll

.PHONY: all clean run dirs

all: dirs $(TARGET)
	@copy_dlls.bat
	@echo Build complete: $(TARGET)

dirs:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@if not exist "$(BUILD_DIR)\core" mkdir "$(BUILD_DIR)\core"
	@if not exist "$(BUILD_DIR)\gui" mkdir "$(BUILD_DIR)\gui"
	@if not exist "$(BUILD_DIR)\engine" mkdir "$(BUILD_DIR)\engine"

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS) -mwindows

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/gui/%.o: $(SRC_DIR)/gui/%.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/core/%.o: $(SRC_DIR)/core/%.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/engine/%.o: $(SRC_DIR)/engine/%.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/app.o: resources/app.rc resources/app.manifest
	windres -i $< -o $@ --input-format=rc

run: all
	./$(TARGET)

clean:
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
