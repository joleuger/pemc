# Probabilistic Executable Model Checker

C++ implementation of algorithms introduced in
"Probabilistic Safety Analysis of Executable Models".

This project is using C++14 and the Meson Build system.
As 3rd party dependencies, Google Test, GSL, Boost, and ThrowAssert are used.

Having installed Clang and Meson, the project can be built as follows:
```
# download dependencies
meson subprojects download
meson build
cd build
ninja
ninja test
```

For a release build use `meson --buildtype release buildrelease` instead of `meson build`.

After compiling, the Knuth dice example can be executed by `./example_dice`. The executable is located in the `build`-directory. Its code is located in the directory `language/pemc_cpp_examples/dice.cc`.


To test the python plugin on macos, run following commands in the build directory:
```
mkdir pypemc
cp pypemc.cpython-39-darwin.so pypemc
cp ../language/pemc_python_examples/dice.py .
python3 dice.py
``` 