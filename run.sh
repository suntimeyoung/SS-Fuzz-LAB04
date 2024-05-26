#!/bin/bash

# 确定 LLVM 的版本和工具的位置
# LLVM_CONFIG="llvm-config-12"
LLVM_CONFIG="llvm-config"
CLANG_COMPILER="clang++-14"

# 检查 llvm-config 工具是否可用
if ! [ -x "$(command -v $LLVM_CONFIG)" ]; then
  echo "llvm-config not found. Please ensure LLVM is correctly installed."
  exit 1
fi

./mkfuzzdir.sh

# 获取编译器和链接器的标志
CXXFLAGS=$($LLVM_CONFIG --cxxflags)
LDFLAGS=$($LLVM_CONFIG --ldflags)

# 编译 instrument.cpp 生成共享库 instrument.so
$CLANG_COMPILER $CXXFLAGS -Wl,-znodelete -fno-rtti -fPIC -shared src/instrument.cpp -o instrument.so $LDFLAGS
# $CLANG_COMPILER $CXXFLAGS -Wl,-znodelete -fno-rtti -fPIC -shared src/hello.cpp -o hello.so $LDFLAGS

# 使用 instrument.so 处理 test.cpp
$CLANG_COMPILER -Xclang -load -Xclang ./instrument.so -c src/test.cpp -o test.o
# $CLANG_COMPILER -Xclang -load -Xclang ./instrument.so -c src/test.cpp -o test.ll
# $CLANG_COMPILER -Xclang -load -Xclang ./hello.so -c src/test.cpp -o test_hello.o

# 编译 instrument_func.cpp, loop.cpp 生成链接文件 instrument_func.o, loop.o
$CLANG_COMPILER -c -Isrc/instrument_func.hpp src/instrument_func.cpp -o instrument_func.o
$CLANG_COMPILER -c -Isrc/loop.hpp src/loop.cpp -o loop.o
$CLANG_COMPILER -c src/main.cpp -o main.o

$CLANG_COMPILER instrument_func.o test.o loop.o -o bin/test

$CLANG_COMPILER loop.o main.o -o bin/main


rm -rf *.o *.so
cp -r bin/* /tmp/.fuzzlab/bin/

