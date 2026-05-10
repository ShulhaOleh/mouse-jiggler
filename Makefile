# Makefile for Mouse Jiggler (Cross-platform, x64 only)

# Compiler settings
CXX := g++
VERSION ?= dev
STATIC  ?= 0
CXXFLAGS := -std=c++23 -Iincludes -DPROJECT_VERSION=\"$(VERSION)\"
WINDRES := windres
SOURCES = src/main.cpp \
          src/jiggler.cpp \
          src/localization.cpp \
          src/updater.cpp \
          src/commands/command_listener.cpp \
          src/commands/commands.cpp \
          src/commands/command_registry.cpp \
          src/logger.cpp
RES := src/mouse-jiggler.rc
RES_OBJ := src/mouse-jiggler.res.o
OBJ := $(SOURCES:.cpp=.o)

TARGET := mouse-jiggler
RELEASE_DIR := release
INSTALL_DIR := /usr/local/bin
UDEV_RULES_DIR := /etc/udev/rules.d

# Platform detection
ifeq ($(OS),Windows_NT)
    PLATFORM := Windows
    RM := cmd /C "if exist $(RELEASE_DIR) rmdir /S /Q $(RELEASE_DIR)"
    MKDIR := cmd /C "if not exist $(RELEASE_DIR) mkdir $(RELEASE_DIR)"
    EXE_EXT := .exe
else
    PLATFORM := Linux
    RM := rm -rf
    MKDIR := mkdir -p $(RELEASE_DIR)
    EXE_EXT :=
endif

# Default target - build for current platform only
.PHONY: all clean run help windows linux native setup install uninstall

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
	$(CXX) $(CXXFLAGS) -static -m64 $(OBJ) $(RES_OBJ) -lstdc++exp -o $(RELEASE_DIR)/$(TARGET)$(EXE_EXT)
ifeq ($(OS),Windows_NT)
	@cmd /C "if exist src\\*.o del /Q src\\*.o"
	@cmd /C "if exist src\\commands\\*.o del /Q src\\commands\\*.o"
	@cmd /C "if exist src\\*.res.o del /Q src\\*.res.o"
endif


# Linux x64 build
linux: $(OBJ)
	@echo "Building Linux x64..."
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) $(if $(filter 1,$(STATIC)),-static) -m64 -pthread $(OBJ) -o $(RELEASE_DIR)/$(TARGET)$(EXE_EXT)
	@rm -f $(OBJ)

# Compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile .rc to .rc.o (Windows resources)
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
	@$(RELEASE_DIR)/$(TARGET)$(EXE_EXT)
else
	@$(MAKE) linux
	@./$(RELEASE_DIR)/$(TARGET)$(EXE_EXT)
endif

install:
	@echo "Installing mouse-jiggler..."
	install -Dm755 $(RELEASE_DIR)/$(TARGET) $(INSTALL_DIR)/$(TARGET)
	install -Dm644 rules/99-mouse-jiggler.rules $(UDEV_RULES_DIR)/99-mouse-jiggler.rules
	udevadm control --reload-rules
	udevadm trigger
	@echo "Done. Log out and back in if /dev/uinput was not yet accessible."

uninstall:
	@echo "Uninstalling mouse-jiggler..."
	rm -f $(INSTALL_DIR)/$(TARGET)
	rm -f $(UDEV_RULES_DIR)/99-mouse-jiggler.rules
	udevadm control --reload-rules
	@echo "Done."

setup:
	pip install --user pre-commit
	pre-commit install

help:
	@echo ""
	@echo "  Mouse Jiggler — $(PLATFORM) x86_64"
	@echo ""
	@echo "  Build"
	@echo "  -----"
	@echo "  make              Build for current platform"
	@echo "  make linux        Build Linux x86_64"
	@echo "  make windows      Build Windows x86_64"
	@echo "  make run          Build and run"
	@echo "  make clean        Remove build artifacts"
	@echo ""
	@echo "  Install  (Linux, requires sudo)"
	@echo "  -----"
	@echo "  make install      Install binary and udev rule to /usr/local/bin"
	@echo "  make uninstall    Remove binary and udev rule"
	@echo ""
	@echo "  Dev"
	@echo "  -----"
	@echo "  make setup        Install git hooks (run once)"
	@echo "  make help         Show this message"
	@echo ""
