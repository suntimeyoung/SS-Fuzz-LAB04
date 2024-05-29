#!/bin/bash

# 确定 LLVM 的版本和工具的位置
LLVM_CONFIG="llvm-config-12"
CLANG_COMPILER="clang++-12"
# LLVM_CONFIG="llvm-config-14"
# CLANG_COMPILER="clang++-14"

# 检查 llvm-config 工具是否可用
if ! [ -x "$(command -v $LLVM_CONFIG)" ]; then
  echo "llvm-config not found. Please ensure LLVM is correctly installed."
  exit 1
fi

# 获取编译器和链接器的标志
CXXFLAGS=$($LLVM_CONFIG --cxxflags)
LDFLAGS=$($LLVM_CONFIG --ldflags)

if [ ! -d bin/ ]; then
    mkdir bin/
fi

if [ ! -d /tmp/.fuzzlab/ ]; then
    mkdir /tmp/.fuzzlab/
fi

if [ ! -d /tmp/.fuzzlab/bin/ ]; then
    mkdir /tmp/.fuzzlab/bin/
fi

if [ ! -d /tmp/.fuzzlab/testcases/ ]; then
    mkdir /tmp/.fuzzlab/testcases/
fi

if [ ! -d build/ ]; then
    mkdir build/
fi

rm /tmp/.fuzzlab/testcases/*

# 编译 instrument.cpp 生成共享库 instrument.so
$CLANG_COMPILER $CXXFLAGS -Wl,-znodelete -fno-rtti -fPIC -shared llvm-mode/instrument.cpp -o instrument.so $LDFLAGS
# $CLANG_COMPILER $CXXFLAGS -Wl,-znodelete -fno-rtti -fPIC -shared src/hello.cpp -o hello.so $LDFLAGS

# 使用 instrument.so 处理 test.cpp
$CLANG_COMPILER -Xclang -load -Xclang ./instrument.so -c examples/test.cpp -o test.o
# $CLANG_COMPILER -Xclang -load -Xclang ./instrument.so -S -emit-llvm examples/test.cpp -o test.ll
# $CLANG_COMPILER -Xclang -load -Xclang ./hello.so -c examples/test.cpp -o test_hello.o

# 编译 instrument_func.cpp 生成链接文件 instrument_func.o
$CLANG_COMPILER -c llvm-mode/instrument_func.cpp -o instrument_func.o
$CLANG_COMPILER -c -Isrc/loop.hpp src/loop.cpp -o loop.o
# $CLANG_COMPILER -S -emit-llvm llvm-mode/instrument_func.cpp -o instrument_func.ll

$CLANG_COMPILER -g loop.o instrument_func.o test.o -o bin/test

rm -rf *.o *.so

cd build && cmake .. && make && cd ..

cp -r bin/* /tmp/.fuzzlab/bin/

