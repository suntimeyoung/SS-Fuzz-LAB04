#include <cstdio>
#include <iostream>

void logBasic(long long condition) {
    static FILE *logFile = NULL;
    if (!logFile) {
        logFile = fopen("./branch_log.txt", "w");
    }
    fprintf(logFile, "Branch address: %lld\n", condition);
}
