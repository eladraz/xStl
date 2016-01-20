![unmaintained](http://img.shields.io/badge/status-unmaintained-red.png)

xStl
====
xStl is a cross-compile, cross-platform C++ library that implements a set of generic templates for easy development.
The library includes an easy abstraction layer for OS-dependent code.

Requirements
============
Windows
-------
Need [Visual Studio](http://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx) Express 2010 or higher, or Visual Studio 2010 or higher.

Ubuntu (Debian)
---------------
`sudo apt-get install automake autoconf libtool build-essential git-core`

CentOS/Fedora/Redhat
--------------------
* Building tools
    `sudo yum install autoconf automake gcc-c++`
    `sudo dnf install autoconf automake gcc-c++`
> ***Some distributions of CentOS have a problem with libstd++ (need to check). For some reason the symbol __cxa_get_exception_ptr is not defined.***

ArchLinux
---------
* Building tools
    `sudo pacman -S base-devel automake autoconf git`
    `sudo pacman -S gcc-multilib libtool-multilib binutils-multilib`

Cygwin
------
Install [cygwin](http://www.cygwin.com/) with the following packages:
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
Windows
-------
In order to pass variable arguments to [Visual Studio](http://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx), you need to declare the following system argument:
* XSTL_PATH (for example: `C:\WORK\github\xStl`)

> To add system variables you should:
>> * Right-click **My Computer**, and then click **Properties**.
>> * Click the **Advanced** tab.
>> * Click **Environment variables**.
>> * Click one the following options, for either a user or a system variable:
>>    * Click *New* to add a new variable name (e.g. `XSTL_PATH`) and value (e.g. `C:\WORK\github\xStl`).
>>    * Click an existing variable, and then click *Edit* to change its name or value.
>>    * Click an existing variable, and then click *Delete* to remove it.

In order to build the xStl library, open `xStl.sln` solution project with [Visual Studio](http://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx).
In Visual Studio's configuration manager, choose the desired build configuration (Win32/x64/Debug/Release) and build `xStl` or `xStl_test`.

Linux
-----
### Compilation Shortcut (build.sh script)
In order to compile xStl you can simply run `build.sh`:
```
#/bin/sh
PWD=`pwd`
./autogen.sh && ./configure [--prefix=${PWD}/out] [--enable-debug] [--enable-unicode] [--enable-tests] && make -j4 && make install
```

### Compile
1. Run `./autogen.sh` in order to generate `Makefile` configure script.
2. Run `./configure` to generate `Makefile` (see **Configure Argument** section, below).
3. Run `make` and `make install`.

#### Configure Argument
* --enable-debug      Compile with debugging flags
* --enable-unicode    Compile with UNICODE support
* --enable-tests      Compile xStl_tests

### Cross-Compile
In order to cross-compile (ARM, for example):
```
./autogen.sh
./configure --build= --host=arm-none-linux-gnueabi
```
Please note that it is important to have a PATH to the cross-compiler bin folder (e.g. `arm-none-linux-gnueabi-g++`).

License
=======
Please see LICENSE file
