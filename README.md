# Probabilistic Executable Model Checker

C++ implementation of algorithms introduced in
"Probabilistic Safety Analysis of Executable Models".


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

