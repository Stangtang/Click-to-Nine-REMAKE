# Click to Nine: The Prequel

A simple game about clicking your way to nine made with raylib. **Sound on!**

The prequel to the popular hit [Click to Ten](https://store.steampowered.com/app/1910580/click_to_ten/).

As this is a difficult game, you may choose to save your progress upon rage quitting to pick up where you left off.

[My personal high score](media/preview.mp4)

## Supported platforms

- Windows
- Linux
- macOS

## Build instructions

### VS Code (all platforms)

Install a compiler toolchain for your platform before building; VS Code alone does not include one.

1. Open the project folder in VS Code.
2. Run the build task with **Ctrl+Shift+B** or **F5**

### Windows: MinGW-w64

From the project root, run:

```sh
make
```

Use a recent version of **MinGW-w64** rather than the older MinGW. You can get it from [w64devkit](https://github.com/skeeto/w64devkit/releases) or use the version installed with the raylib installer.

If you installed raylib with its installer, add `C:\raylib\w64devkit\bin` to your `PATH` so the compiler can be found. You do not need to install a second MinGW-w64 toolchain.

### Windows: Microsoft Visual Studio 2026

Build instructions for this toolchain are not documented yet.

### Linux

From the project root, run:

```sh
cd build
./premake5 gmake
cd ..
make
```

### macOS

From the project root, run:

```sh
cd build
./premake5.osx gmake
cd ..
make
```

## Output

The built game is placed in `bin/`.
