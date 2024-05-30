# CHINA FUZZ LOOP

## Brief Introduction

This project is for BUAA CST SS lab4.

## Toolchain

| tool          | version                                                 |
| :---          | :----                                                   |
| Linux         | 22.04.1-Ubuntu x86_64 GNU/Linux (ensure **Unix-like**!) |
| Clang/Clang++ | Ubuntu clang version 12.0.1-19ubuntu3 (**not 14**!)     |
| llvm-config   | 12.0.1 (**not 14.0.0**!)                                |
| make          | GNU Make 4.3 x86_64                                     |
| cmake         | cmake version 3.22.1 (ensure **>= 3.10**)               |


## Quickstart

1. Run:
   ```
   $ ./run.sh
   ```
2. Then:
   ```
   $ bin/fuzz_main
   ```
3. For more clarity:
   ```
   $ bin/fuzz_main | grep testing
   $ bin/fuzz_main | grep round
   $ bin/fuzz_main | grep Parent
   $ bin/fuzz_main | grep Child
   ...
   ```

## Ideas

See `./src/Ideas.md`.

