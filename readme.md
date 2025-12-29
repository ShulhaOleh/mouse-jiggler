<div align="center">

![Demo](resources/mouse-jiggler-bg.png)

# Mouse Jiggler

</div>

Simple C++ tool that moves your cursor a few pixels over a certain period of time for Windows and Linux (x64).

## Purpose

If you need to prevent a utility (a program that tracks your mouse movements) from detecting that you are inactive, this small utility will help you do that.

## Project structure

<!-- STRUCTURE_START -->

```
mouse-jiggler/
├── .pre-commit-config.yaml
├── Makefile
├── hooks
│   └── generate_structure.py
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
- g++ compiler
- X11 development libraries
- Install required packages:
  
  **Ubuntu/Debian:**
```bash
  sudo apt update
  sudo apt install build-essential libx11-dev
```

## Known issues

* In the Linux version, the cursor sticks to the upper left corner of the monitor screen.
* Currently `localization.cpp` has no Linux realization.