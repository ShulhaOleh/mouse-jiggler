# Makefile for Mouse Jiggler (Cross-platform, x64 only)

# Compiler settings
CXX := g++
CXXFLAGS := -std=c++14 -Iincludes
WINDRES := windres
SOURCES = src/main.cpp \
          src/jiggler.cpp \
          src/localization.cpp \
          src/commands/command_listener.cpp \
          src/commands/commands.cpp \
          src/commands/command_registry.cpp
RES := src/mouse-jiggler.rc
RES_OBJ := src/mouse-jiggler.res.o
OBJ := $(SOURCES:.cpp=.o)

TARGET := mouse-jiggler
RELEASE_DIR := release

# Platform detection
ifeq ($(OS),Windows_NT)
    PLATFORM := Windows
    RM := cmd /C "if exist $(RELEASE_DIR) rmdir /S /Q $(RELEASE_DIR)"
    MKDIR := cmd /C "if not exist $(RELEASE_DIR) mkdir $(RELEASE_DIR)"
	COPY_LOCALES := cmd /C "xcopy /E /I /Y locales $(RELEASE_DIR)\locales"
    EXE_EXT := .exe
else
    PLATFORM := Linux
    RM := rm -rf
    MKDIR := mkdir -p $(RELEASE_DIR)
    EXE_EXT :=
endif

# Default target - build for current platform only
.PHONY: all clean run help windows linux native setup

native:
	@echo "Building for current platform: $(PLATFORM) x64"
ifeq ($(PLATFORM),Windows)
	@$(MAKE) windows
else
	@$(MAKE) linux
endif
	@echo "========================================"
	@echo "$(PLATFORM) x64 build complete!"
	@echo "========================================"

# Windows x64 build
windows: $(OBJ) $(RES_OBJ)
	@echo "Building Windows x64..."
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) -static -m64 $(OBJ) $(RES_OBJ) -o $(RELEASE_DIR)/$(TARGET)-windows-x64$(EXE_EXT)
	@$(COPY_LOCALES)
ifeq ($(OS),Windows_NT)
	@cmd /C "if exist src\\*.o del /Q src\\*.o"
	@cmd /C "if exist src\\commands\\*.o del /Q src\\commands\\*.o"
	@cmd /C "if exist src\\*.res.o del /Q src\\*.res.o"
endif


# Linux x64 build
linux: $(OBJ)
	@echo "Building Linux x64..."
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) -m64 -pthread $(OBJ) -o $(RELEASE_DIR)/$(TARGET)-linux-x64$(EXE_EXT) -lX11
	@rm -f $(OBJ)

# Compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile .rc to .rc.o ((Windows resources))
$(RES_OBJ): $(RES)
ifeq ($(OS),Windows_NT)
	$(WINDRES) $< -O coff -o $@
endif


clean:
	@echo "Cleaning build files..."
	@$(RM)
ifeq ($(OS),Windows_NT)
	@cmd /C "if exist src\\*.o del /Q src\\*.o"
	@cmd /C "if exist src\\commands\\*.o del /Q src\\commands\\*.o"
	@cmd /C "if exist src\\*.res.o del /Q src\\*.res.o"
else
	@rm -f src/*.o src/commands/*.o
endif
	@echo "Clean complete."

run:
ifeq ($(OS),Windows_NT)
	@$(MAKE) windows
	@$(RELEASE_DIR)/$(TARGET)-windows-x64$(EXE_EXT)
else
	@$(MAKE) linux
	@./$(RELEASE_DIR)/$(TARGET)-linux-x64$(EXE_EXT)
endif

setup:
	pip install --user pre-commit
	pre-commit install

help:
	@echo "Mouse Jiggler Makefile"
	@echo "============================================"
	@echo "Detected platform: $(PLATFORM)"
	@echo ""
	@echo "Available targets:"
	@echo "  make         - Build x64 for current platform ($(PLATFORM))"
	@echo "  make windows - Build Windows x64"
	@echo "  make linux   - Build Linux x64"
	@echo "  make clean   - Remove all build files"
	@echo "  make run     - Build and run x64 build"
	@echo "  make setup   - Install all hooks for git (need to execute only once)"
	@echo "  make help    - Show this help"
