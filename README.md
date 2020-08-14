# Spimbot

This is a revamped and refactored version of the CS233 QtSpimbot. In this version, many of the original QtSpim types have been properly converted to their equivalent C++ types.

Spimbot is a game that where the inputs are controlled by code. As of now, there is only the only supported controller format is MIPS.

## How to build

```bash
~/package $ mkdir build
~/package $ cd build
~/package/build $ cmake .. -DCMAKE_INSTALL_PREFIX=..
~/package/build $ make
```

## Design Goals

- Keep the UI, the game world, and the emulator as separate as possible
- Minimize the reliance on Qt and Boost and design with the intention to add ARM and Electron support
- Keep the design as sleek as possible and minimize circular dependencies
- Use more widely used and supported tools like CMake rather than qmake

## Notes about developing

Unlike most programs, we have strict performance requirements. As we hold a tournament at the end of the semester, we need to be able to run a lot of matches in a linear fashion within an hour. Strive to compute 10,000,000 cycles in about 12 seconds. Unlike other games, we run as fast as possible. There is no framerate and everything is dependent on the computer speed. So, costs like virtual function calls cannot be "hidden" by the framerate.

This codebase should be ported to Rust as soon as it gets good cross-platform GUI support (pro: variants and nicer syntax) or C++20 as soon as compilers support it (pro: reflection / variants fixes / filesystem / concepts / ranges / spaceship / modules / coroutines would be very nice).

### Project Structure

- UI, World, Systems are structured after the model view controller architecture with the Engine serving as the event loop
- Controllers in this case are effectively clients that operate on fixed time intervals.
- Controllers exist between the CPU and memory and serve as a way to simulate the memory bus.

### Nice Features

- Cache hits and misses
- Alternative UI options like a HTML frontend or a Vscode extension
- Support for other languages and may other types of bytecode.

### Allowable Boost Libraries

#### Mandatory

- Boost.PropertyTree
  - As of C++17, there is not much support for serialization and deserialization from JSON, which will be used in log files. Once compilers implement to C++20 and libraries allow for JSON serialization, drop this dependency.
- Boost.Random
  - As C++ does not standardize the distributions of int std::random, we use boost to do this for us.

#### Collection libaries

- Boost.circular_buffer
  - This is a convenience library for creating ring buffers. While not completely necessary, it is nice to have until we roll our own implementation.
- Bimap
  - Also a convenience library for bidirectional maps which are searchable by key and value.

### Allowable Qt Modules

- core
- gui
- widgets
- printsupport

### Misc dependencies

- Catch2 testing framework
- spdlog

## Quick Licensing Information

All licensing is located in the docs folder.

- Boost is boost licensed
- Original QtSpim was BSD licensed
- Qt is LGPL
- spdlog is MIT licensed
- Catch2 is Boost licensed
- Amethyst for inspiration under MIT license
