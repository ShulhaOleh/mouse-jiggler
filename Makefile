# Makefile for Mouse Jiggler (Cross-platform, x64 only)

# Compiler settings
CXX := g++
CXXFLAGS := -std=c++14
SOURCE := main.cpp
RELEASE_DIR := release

# Platform detection
ifeq ($(OS),Windows_NT)
    PLATFORM := Windows
    RM := cmd /C "if exist $(RELEASE_DIR) rmdir /S /Q $(RELEASE_DIR)"
    MKDIR := cmd /C "if not exist $(RELEASE_DIR) mkdir $(RELEASE_DIR)"
else
    PLATFORM := Linux
    RM := rm -rf
    MKDIR := mkdir -p $(RELEASE_DIR)
endif

# Targets
.PHONY: clean run help windows linux

# Default target - build for current platform only
native:
	@echo "Building for current platform: $(PLATFORM) x64"
	@echo ""
ifeq ($(PLATFORM),Windows)
	@$(MAKE) windows
else
	@$(MAKE) linux
endif
	@echo ""
	@echo "========================================"
	@echo "$(PLATFORM) x64 build complete!"
	@echo "========================================"

# Windows x64 build
windows:
	@echo "Building Windows x64..."
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) -static -m64 $(SOURCE) -o $(RELEASE_DIR)/mouse-jiggler-windows-x64.exe

# Linux x64 build
linux:
	@echo "Building Linux x64..."
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) -m64 -pthread $(SOURCE) -o $(RELEASE_DIR)/mouse-jiggler-linux-x64 -lX11

clean:
	@echo "Cleaning build files..."
	@$(RM)
	@echo "Clean complete."

run:
ifeq ($(OS),Windows_NT)
	@echo "Building and running Windows x64..."
	@$(MAKE) windows
	@$(RELEASE_DIR)\mouse-jiggler-windows-x64.exe
else
	@echo "Building and running Linux x64..."
	@$(MAKE) linux
	@./$(RELEASE_DIR)/mouse-jiggler-linux-x64
endif

help:
	@echo "Mouse Jiggler Makefile"
	@echo "============================================"
	@echo "Detected platform: $(PLATFORM)"
	@echo ""
	@echo "Available targets:"
	@echo "  make         - Build x64 for current platform ($(PLATFORM))"
	@echo "  make all     - Build for current platform (use on each OS separately)"
	@echo "  make windows - Build Windows x64"
	@echo "  make linux   - Build Linux x64"
	@echo "  make clean   - Remove all build files"
	@echo "  make run     - Build and run x64 build"
	@echo "  make help    - Show this help"
	@echo ""
	@echo "Output directory: $(RELEASE_DIR)/"
