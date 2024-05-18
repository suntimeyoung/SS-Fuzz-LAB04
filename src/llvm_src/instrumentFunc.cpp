#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>
#include <cstdio>
#include <set>

#define LOG_FILE "log/branch_log.txt"
#define FIFO_DATA "/tmp/fuzz_fifo_data"
#define FIFO_INST "/tmp/fuzz_fifo_inst"
#define BUFFER_SIZE 1024
#define TEST_INSTANCE_NUM 20
#define CONTINUE_INST 0
#define WAIT_INST 1

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
    int inst;
    int fork_num = 0;
    char buffer[BUFFER_SIZE + 1];

    int data_pipe_fd = open(FIFO_DATA, O_RDONLY);
    if (data_pipe_fd == -1) {
        fprintf(stderr, "Open error on named pipe: %s\n", FIFO_DATA);
        exit(EXIT_FAILURE);
    }

    while (true) {

        if (fork_num++ == TEST_INSTANCE_NUM) {
            // sleep(10);
            exit(EXIT_SUCCESS);
        }
        pid_t pid = fork();

        if (pid > 0) {
            // parent

            /** send or get some instructions? */

        } else {
            // child

            // get test input file path (stored in `buffer`) from data pipe.
            if (read(data_pipe_fd, buffer, BUFFER_SIZE) > 0) {
                // pipe test input file to `stdin`.
                freopen(buffer, "r", stdin);
                std::cout << "\n(times:"<< times << ") Child (pid:" << getpid() << "): pipe " << buffer << " to the stdin of child." << std::endl;
            } else {
                fprintf(stderr, "Read error on pipe %s\n", FIFO_DATA);
                exit(EXIT_FAILURE);
            }

            return ;
        }

        times ++;
    }

    close(data_pipe_fd);
    exit(EXIT_SUCCESS);
}

