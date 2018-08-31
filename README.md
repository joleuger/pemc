# Probabilistic Executable Model Checker

C++ implementation of algorithms introduced in
"Probabilistic Safety Analysis of Executable Models".


```
mkdir -p external
cd external
git clone https://github.com/google/googletest.git
git clone https://github.com/Microsoft/GSL.git
git clone https://github.com/Softwariness/ThrowAssert.git
cd ..

mkdir build 
cd build
meson .. .
meson test
```

