#!/bin/bash

# 确定 LLVM 的版本和工具的位置
# LLVM_CONFIG="llvm-config-12"
LLVM_CONFIG="llvm-config"

# 检查 llvm-config 工具是否可用
if ! [ -x "$(command -v $LLVM_CONFIG)" ]; then
  echo "llvm-config not found. Please ensure LLVM is correctly installed."
  exit 1
fi

# 获取编译器和链接器的标志
CXXFLAGS=$($LLVM_CONFIG --cxxflags)
LDFLAGS=$($LLVM_CONFIG --ldflags)

# 编译 instrument.cpp 生成共享库 instrument.so
clang++-12 $CXXFLAGS -Wl,-znodelete -fno-rtti -fPIC -shared ./src/llvm_src/instrument.cpp -o ./tmp/so/instrument.so $LDFLAGS
# clang++-12 $CXXFLAGS -Wl,-znodelete -fno-rtti -fPIC -shared ./src/llvm_src/hello.cpp -o ./tmp/so/hello.so $LDFLAGS

# 使用 instrument.so 处理 test.cpp
clang++-12 -Xclang -load -Xclang ./tmp/so/instrument.so -c test.cpp -o ./tmp/o/test.o
# clang++-12 -Xclang -load -Xclang ./tmp/so/instrument.so -c test.cpp -o ./tmp/o/test.ll
# clang++-12 -Xclang -load -Xclang ./tmp/so/hello.so -c test.cpp -o ./tmp/o/test_hello.o

# 编译 instrumentFunc.cpp 生成链接文件 instrumentFunc.o
clang++-12 -c ./src/llvm_src/instrumentFunc.cpp -o ./tmp/o/instrumentFunc.o

clang++-12 ./tmp/o/instrumentFunc.o ./tmp/o/test.o -o bin/test
