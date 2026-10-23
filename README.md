![maintained](http://img.shields.io/badge/status-maintained-brightgreen.png)
[![CI](https://github.com/eladraz/xStl/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/eladraz/xStl/actions/workflows/ci.yml)
[![coverity status](https://scan.coverity.com/projects/10737/badge.svg)](https://scan.coverity.com/projects/eladraz-xstl)

xStl
====
xStl is a cross-compile, cross-platform C++ library that implements a set of generic templates for easy development.
The library includes an easy abstraction layer for OS-dependent code.

Requirements
============
Windows
-------
Requires [Visual Studio](http://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx) Express 2010 or newer, or Visual Studio 2010 or newer.

Ubuntu (Debian)
---------------
`sudo apt-get install automake autoconf libtool build-essential git-core`

CentOS/Fedora/Red Hat
--------------------
* Building tools
    `sudo yum install autoconf automake gcc-c++`
    `sudo dnf install autoconf automake gcc-c++`
> ***Some distributions of CentOS have a problem with libstdc++ (to be verified). For some reason the symbol __cxa_get_exception_ptr is not defined.***

ArchLinux
---------
* Building tools
    `sudo pacman -S base-devel automake autoconf git`
    `sudo pacman -S gcc-multilib libtool-multilib binutils-multilib`

macOS
-----
Tested on macOS 26 (Apple Silicon/arm64) with Apple clang 21.
* CMake build (recommended)
    `xcode-select --install` and `brew install cmake`
* Autotools build (optional, not yet verified on macOS)
    `brew install autoconf automake libtool` (`autogen.sh` uses `glibtoolize` on macOS)

Cygwin
------
Install [Cygwin](http://www.cygwin.com/) with the following packages:
* gcc
* g++
* automake
* autoconf
* libtool

Setting Up Environment
======================
Git
---
```
git clone https://github.com/eladraz/xStl
```

How to Build
============
CMake
-----
CMake is a cross-platform build system for many platforms and systems.
You can read more about it [here](https://cmake.org/install/)

The library and the tests are two separate CMake projects. Build the library
first; it is written to `out/lib` (static `libxStlcm_static.a` and shared `libxStlcm.so`/`.dylib`).
The tests project then finds it there.

```
# Build the library (build type defaults to Debug)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# Build and run the tests (binary is written to tests/out/bin)
cmake -S tests -B tests/build -DCMAKE_BUILD_TYPE=Release
cmake --build tests/build -j
cd tests && ./out/bin/xstl_tests
```

The tests print `OK!` and exit with 0 when every module passes. `Trace: CHECK FAILED`
lines in the output are expected: some tests deliberately trigger assertions.

To use Ninja instead of Make, add `-G Ninja` to the configure commands.

Windows
-------
In order to pass variables to [Visual Studio](http://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx), you need to declare the following environment variable:
* XSTL_PATH (for example: `C:\WORK\github\xStl`)

> To add environment variables:
>> * Right-click **My Computer**, and then click **Properties**.
>> * Click the **Advanced** tab.
>> * Click **Environment variables**.
>> * Click one of the following options, for either a user or a system variable:
>>    * Click *New* to add a new variable name (e.g. `XSTL_PATH`) and value (e.g. `C:\WORK\github\xStl`).
>>    * Click an existing variable, and then click *Edit* to change its name or value.
>>    * Click an existing variable, and then click *Delete* to remove it.

In order to build the xStl library, open the `xStl.sln` solution with [Visual Studio](http://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx).
In Visual Studio's configuration manager, choose the desired build configuration (Win32/x64/Debug/Release) and build `xStl` or `xStlTests`.

Linux / macOS
-------------
### Compilation Shortcut (build.sh script)
In order to compile xStl you can simply run `build.sh`:
```
#!/bin/sh
PWD=`pwd`
./autogen.sh && ./configure [--prefix=${PWD}/out] [--enable-debug] [--enable-unicode] [--enable-tests] && make -j4 && make install
```

### Compile
1. Run `./autogen.sh` in order to generate the `configure` script.
2. Run `./configure` to generate `Makefile` (see the **Configure Arguments** section, below).
3. Run `make` and `make install`.

#### Configure Arguments
* --enable-debug      Compile with debugging flags
* --enable-unicode    Compile with UNICODE support
* --enable-tests      Compile xStl_tests

### Cross-Compile
In order to cross-compile (ARM, for example):
```
./autogen.sh
./configure --build= --host=arm-none-linux-gnueabi
```
Please note that the cross-compiler `bin` folder must be in your PATH (e.g. `arm-none-linux-gnueabi-g++`).

API Documentation
=================
The API reference is generated with [Doxygen](https://www.doxygen.nl/) from the
comments in `Include/xStl`. Install Doxygen (the documentation was generated and
checked with version 1.18.0), then run:

```
# Windows (PowerShell): winget install DimitriVanHeesch.Doxygen
powershell -File docs\build-docs.ps1

# Linux / macOS (GNU make)
make -f docs/Makefile docs-all
```

The HTML pages are written to `docs/build/html` (open `index.html`) and the XML
to `docs/build/xml` (for Sphinx/Breathe and similar tools). Other targets:
`docs-html`, `docs-xml`, `docs-api` (HTML and XML), `docs-pdf` (PDF manual, needs a\nLaTeX installation: `pdflatex`, `makeindex`, `epstopdf`), `docs-check` (fails on any
Doxygen warning) and `docs-clean`. See `docs/README.md` for the details.

License
=======
Please see the LICENSE file
