Note: Currently not working

Download precompiled Boost binary from https://sourceforge.net/projects/boost/files/boost-binaries/

Set boost root on command line
```
SET BOOST_ROOT=C:\local\boost_1_70_0
```

Load the environment variables for visual studio
```
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat
```
and then load the environment variables for clang
```
set "LLVMPath=C:\Program Files\LLVM"
set "PATH=%PATH%;%LLVMPath%\bin
SET CXX=clang++
```

Now you can compile as usual
```
meson subprojects download
meson build
cd build
ninja
ninja test
```