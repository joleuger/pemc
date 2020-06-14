Building and running pemc under Windows.

# Dependencies

## Visual Studio
Download the Visual Studio Build Tools 2019 from https://visualstudio.microsoft.com/de/visual-cpp-build-tools/.
Alternatively, installing the Build Tools should work by installing Visual Studio. Select at least a Windows SDK, a compiler, and CMake. Currently, Clang on Windows depends on Visual Studio.

## Ninja and meson
1. Download Ninja and Meson. There are thre options:

a) Python and pip (tested releases):
* Install Python 3 from https://www.python.org/. Note that pip must be enabled.
* Open a terminal window as administrator.
* Run "pip3 install meson==0.54.2"
* Run "pip install ninja==1.9.0.post1". The old 

b) Python and pip (current releases, might requitre an automatic compilation):
* Install Python 3 from https://www.python.org/. Note that pip must be enabled.
* Open a terminal window as administrator.
* Load the environment variables for Visual Studio:
```
C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat
# or
C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat
```
* Run "pip3 install meson" and "pip install ninja". Might not work if the correct tool chain is not installed.

c) MSI-Package:
* Downloaad and install msi-package from https://mesonbuild.com/Getting-meson.html. Note that version 0.54.2 has a bug with cmake.

## Clang
Download Clang (win64) from https://releases.llvm.org/.

## Boost

Download and install the precompiled Boost binary from https://sourceforge.net/projects/boost/files/boost-binaries/.
Select the version that fits to the installed Visual studio tool chain.

# Compile

Open a terminal window without administrator permissions.

Set boost root on command line:
```
SET BOOST_ROOT=C:\local\boost_1_73_0
set PATH=%PATH%;C:\local\boost_1_73_0\lib64-msvc-14.2
```

Load the environment variables for visual :
```
C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat
# or
C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat
```


Skip for now, because clang on Windows is currently not supported: Then, load the environment variables for clang:
```
set "LLVMPath=C:\Program Files\LLVM"
set PATH=%PATH%;%LLVMPath%\bin
SET CXX=clang++
```.

Now you can compile as usual:
```
meson subprojects download
meson build
cd build
ninja
ninja test
```


# Additional Notes

* https://llvm.org/docs/GettingStartedVS.html