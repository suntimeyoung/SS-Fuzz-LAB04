#!/bin/bash

# 确定 LLVM 的版本和工具的位置
LLVM_CONFIG="llvm-config-12"
CLANG_COMPILER="clang++-12"
TEST_PROG=$1

if [ "$TEST_PROG" = "" ]; then
    TEST_PROG="test.cpp"
fi

if  [ ! -f examples/$TEST_PROG ]; then
    echo "Please ensure the test program path is right."
    exit
fi


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
    mkdir build/instrument_so/
    $CLANG_COMPILER $CXXFLAGS -Wl,-znodelete -fno-rtti -fPIC -shared llvm-mode/instrument.cpp -o build/instrument_so/instrument.so $LDFLAGS
    $CLANG_COMPILER -c llvm-mode/instrument_func.cpp -o build/instrument_so/instrument_func.o
    $CLANG_COMPILER -c -Isrc/loop.hpp src/loop.cpp -o build/instrument_so/loop.o
else
    if [ ! -d build/instrument_so/ ] || [ -z "$(ls -A build/instrument_so/)" ]; then
        mkdir build/instrument_so/
        $CLANG_COMPILER $CXXFLAGS -Wl,-znodelete -fno-rtti -fPIC -shared llvm-mode/instrument.cpp -o build/instrument_so/instrument.so $LDFLAGS
        $CLANG_COMPILER -c llvm-mode/instrument_func.cpp -o build/instrument_so/instrument_func.o
        $CLANG_COMPILER -c -Isrc/loop.hpp src/loop.cpp -o build/instrument_so/loop.o
    fi
fi

if [ ! -z "$(ls -A /tmp/.fuzzlab/testcases/)" ]; then
    rm /tmp/.fuzzlab/testcases/*
fi

$CLANG_COMPILER -Xclang -load -Xclang build/instrument_so/instrument.so -c examples/$TEST_PROG -o $TEST_PROG.o
$CLANG_COMPILER -g build/instrument_so/loop.o build/instrument_so/instrument_func.o $TEST_PROG.o -o bin/$TEST_PROG.elf

rm -rf *.o *.so

cd build && cmake .. && make && cd ..

cp -r bin/* /tmp/.fuzzlab/bin/

