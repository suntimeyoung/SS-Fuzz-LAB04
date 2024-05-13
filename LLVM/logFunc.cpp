#include <cstdio>
#include <iostream>

extern "C" void logBasic(unsigned long long address) {
    static FILE *logFile = NULL;
    if (!logFile) {
        logFile = fopen("./branch_log.txt", "w");
    }
    fprintf(logFile, "Branch address: %llu\n", address);
}
