<div align="center">

![Demo](resources/mouse-jiggler-bg.png)

# Mouse Jiggler

</div>

Simple C++ tool that moves your cursor a few pixels over a certain period of time for Windows and Linux (x64). On Linux, mouse input is emulated via the kernel's `uinput` interface, which works on both X11 and Wayland.

## Purpose

If you need to prevent a utility (a program that tracks your mouse movements) from detecting that you are inactive, this small utility will help you do that.

## Project structure

<!-- STRUCTURE_START -->

```
mouse-jiggler/
├── hooks/
│   └── pre-commit.py
├── includes/                       # Project header files
│   ├── commands/                   # Subsystem for commands
│   │   ├── command.h
│   │   ├── command_listener.h
│   │   ├── command_registry.h
│   │   └── commands.h
│   ├── jiggler.h
│   ├── localization.h              # Localization system
│   ├── logger.h
│   ├── platform.h                  # Macros for working with platforms
│   └── version.h                   # Version information
├── locales/                        # Translation files
│   ├── en.json
│   ├── ru.json
│   └── uk.json
├── resources/
│   ├── mouse-jiggler-bg.png
│   ├── mouse-jiggler-icon.ico
│   └── mouse-jiggler-icon.png
├── scripts/                        # Utility scripts
│   ├── generate_structure.py       # Project tree generator
│   └── project_comments.json
├── src/                            # Source code
│   ├── commands/                   # Command subsystem implementation
│   │   ├── command_listener.cpp
│   │   ├── command_registry.cpp
│   │   └── commands.cpp
│   ├── jiggler.cpp
│   ├── localization.cpp
│   ├── logger.cpp
│   ├── main.cpp                    # Application entry point
│   └── mouse-jiggler.rc
├── .pre-commit-config.yaml
├── LICENSE
├── Makefile                        # Build file
└── readme.md
```

<!-- STRUCTURE_END -->

## Usage

### Windows
```cmd
mouse-jiggler-windows-x64.exe
```

### Linux
```bash
./mouse-jiggler-linux-x64
```

Enter `q` or `quit` to stop the program.

## Building from Source

### Prerequisites

#### Windows
- [Make](https://gnuwin32.sourceforge.net/packages/make.htm) for Windows
- In the project root, enter `make setup` to install all hooks
- [Python3](https://www.python.org/)
- [MSYS2](https://www.msys2.org/) with MinGW-w64 toolchain
- Install required packages:
```bash
  pacman -S mingw-w64-x86_64-gcc make
```

#### Linux
- Make for Linux
- In the project root, enter `make setup` to install all hooks
- [Python3](https://www.python.org/)
- GCC 14.0.0+
- `uinput` kernel module (enabled by default on most distributions)
- Install required packages:

  **Ubuntu/Debian:**
```bash
  sudo apt update
  sudo apt install build-essential
```

> **Note:** `/dev/uinput` requires elevated permissions. Either run as root, or add your user to the `input` group:
> ```bash
> sudo usermod -aG input $USER
> ```
> Log out and back in for the change to take effect.

## License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.

