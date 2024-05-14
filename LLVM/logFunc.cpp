#include <cstdio>
#include <iostream>
#include <set>

using namespace std;

unsigned long long last_add = 0;
set<unsigned long long> branches;


extern "C" void logBasic(unsigned long long address) {
    branches.insert(address ^ last_add);
    last_add = address;
    static FILE *logFile = NULL;
    if (!logFile) {
        logFile = fopen("./branch_log.txt", "w");
    }
    fprintf(logFile, "Branches count: %lu\n", branches.size());
}

// extern "C" void 

