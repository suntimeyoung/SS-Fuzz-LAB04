#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <set>


unsigned long long last_add = 0;
std::set<unsigned long long> branches;


extern "C" void logBasic(unsigned long long address) {
    // `address ^ last_add` is indistinguishable from `last_add ^ address`.
    // branches.insert((address) ^ last_add);
    // `(address+1) ^ last_add` differs the appearance order of address.
    branches.insert((address+1) ^ last_add);
    last_add = address;
    static FILE *logFile = NULL;
    if (!logFile) {
        logFile = fopen("./branch_log.txt", "w");
    }
    fprintf(logFile, "Branches count: %lu\n", branches.size());
}

extern "C" void forkServer() {
    int times = 3;
    while (times --) {
        pid_t pid = fork();

        if (pid > 0) {
            // parent
            std::cout << "This is parent from pid: " << getpid() << std::endl;
        } else {
            std::cout << "This is child from pid: " << getpid() << std::endl;
            return ;
        }
    }
}

