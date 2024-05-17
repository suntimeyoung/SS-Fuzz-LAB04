#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <iostream>
#include <cstdio>
#include <set>

#define LOG_FILE "log/branch_log.txt"
#define FIFO_FILE "/tmp/fuzz_fifo"

unsigned long long last_add = 0;
std::set<unsigned long long> branches;


extern "C" void logBasic(unsigned long long address) {

    /**
     * `address ^ last_add` is indistinguishable from `last_add ^ address`.
     * `(address+1) ^ last_add` differs the appearance order of address.
    */

    // branches.insert((address) ^ last_add);
    branches.insert((address+1) ^ last_add);

    last_add = address;
    static FILE *logFile = NULL;
    if (!logFile) {
        logFile = fopen(LOG_FILE, "w");
    }
    fprintf(logFile, "Branches count: %lu\n", branches.size());
}

extern "C" void forkServer() {
    int times = 0;
    while (true) {
        pid_t pid = fork();

        if (pid > 0) {
            // parent
            std::cout << "(times: " << times++ << ")This is parent from pid: " << getpid() << std::endl;

            // wait for the child to finish
            int status;
            waitpid(pid, &status, 0);

        } else {
            // child
            std::cout << "(times: "<< times++ << ")This is child from pid: " << getpid() << std::endl;
            int pipe_fd;
            if ((pipe_fd = open(FIFO_FILE, O_RDONLY)) != -1) {
                // dump named pipe `pipe_fd` to the stdin of child process of tested program.
                dup2(pipe_fd, STDIN_FILENO);
                std::cout << "(times: "<< times++ << ")Dumped pipe into stdin of child." << std::endl;
            }

            sleep(10);

            return ;
        }
    }
}

