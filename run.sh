#!/bin/bash

# 确定 LLVM 的版本和工具的位置
LLVM_CONFIG="llvm-config"

# 检查 llvm-config 工具是否可用
if ! [ -x "$(command -v $LLVM_CONFIG)" ]; then
  echo "llvm-config not found. Please ensure LLVM is correctly installed."
  exit 1
fi

# 获取编译器和链接器的标志
CXXFLAGS=$($LLVM_CONFIG --cxxflags)
LDFLAGS=$($LLVM_CONFIG --ldflags)

# 编译 llvm_log.cpp 生成共享库 llvm_log.so
clang++-12 $CXXFLAGS -Wl,-znodelete -fno-rtti -fPIC -shared ./LLVM/llvm_log.cpp -o ./LLVM/llvm_log.so $LDFLAGS
# clang++-12 $CXXFLAGS -Wl,-znodelete -fno-rtti -fPIC -shared ./LLVM/hello.cpp -o ./LLVM/hello.so $LDFLAGS

# 使用 llvm_log.so 处理 test.cpp
clang++-12 -Xclang -load -Xclang ./LLVM/llvm_log.so -c test.cpp -o ./tmp/test.o
# clang++-12 -Xclang -load -Xclang ./LLVM/hello.so -c test.cpp -o ./tmp/test_hello.o

# 编译 logFunc.cpp 生成链接文件 logFunc.o
clang++-12 -c ./LLVM/logFunc.cpp -o ./tmp/logFunc.o

clang++-12 ./tmp/test.o ./tmp/logFunc.o -o test