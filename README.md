# Planes
An Asteroids-like game where you pilot a aircraft and survive as long as you can from enemy aircraft.

## Building
Before building Planes, the following tools must be first installed.

 * CMake (`>= 3.10.2`)
 * Conan (`>= 1.21.1`)
 * GCC (`>= 7.40`)

Note that you may use other C++ compilers, such as Clang, but assurance of a smooth compilation is, for now, only provided when using GCC.

### Downloading Dependencies
Dependency management is handled by Conan. As such, the first step in building Planes involves using Conan. Conan will download the dependencies and generate files that are required in building the project, but change from computer to computer. As such, we will be running Conan in the build tree.

    $ mkdir build
    $ cd build
    $ conan install ..

### Compiling
CMake will handle generating the necessary files for compilation.

    $ cd build  # if not in the build tree already.
    $ cmake ..

Compilation steps will differ from platform to platform. For now, we will only be compiling in Linux.

    $ make
