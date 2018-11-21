# Probabilistic Executable Model Checker

C++ implementation of algorithms introduced in
"Probabilistic Safety Analysis of Executable Models".

This project is using C++14 and the Meson Build system.
As 3rd party dependencies, Google Test, GSL, Boost, and ThrowAssert are used.

Having installed Clang and Meson, the project can be built as follows:
```
mkdir -p external
cd external
git clone https://github.com/google/googletest.git
git clone https://github.com/Microsoft/GSL.git
git clone https://github.com/Softwariness/ThrowAssert.git

#git clone https://github.com/boostorg/range.git
#cd range
#git checkout tags/boost-1.68.0 -b boost-1.68.0
#cd ..

cd ..

mkdir build 
cd build
meson .. .
meson test
```

After compiling, the Knuth dice example can be executed by `./example_dice`. The executable is located in the `build`-directory. Its code is located in the directory `language/pemcCppExamples/dice.cc`.


