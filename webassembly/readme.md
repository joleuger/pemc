https://github.com/prozum/meson-samples

https://github.com/mesonbuild/meson/search?q=gtest&unscoped_q=gtest

# On macOS
```
xcode-select --install
brew install ninja
brew install emscripten
pip3 install meson

# run llvm once
update LLVM_ROOT in ~/.emscripten to /usr/local/Cellar//emscripten/1.38.6/libexec/llvm/bin


meson .. . --cross-file emscripten-osx.txt

```


