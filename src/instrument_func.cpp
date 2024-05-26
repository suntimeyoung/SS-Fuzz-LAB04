#include "loop.hpp"
#include <set>

unsigned long long last_add = 0;
std::set<unsigned long long> branches;


extern "C" void LogBasic(unsigned long long address) {

    branches.insert((address << 1) ^ last_add);
    last_add = address;

    static FILE *logFile = NULL;
    if (!logFile) {
        logFile = fopen(LOG_FILE, "w");
    }
    fprintf(logFile, "Branches count: %lu\n", branches.size());

}

extern "C" void ForkServer() {
    int times = 0;
    char buf[PIPE_BUF_SIZE + 1];
    char inst_buf[PIPE_BUF_SIZE + 1];

    int data_pipe_fd = open(FIFO_DATA, O_RDONLY);
    if (data_pipe_fd == -1) {
        fprintf(stderr, "Open error on named pipe: %s\n", FIFO_DATA);
        exit(EXIT_FAILURE);
    }

    int inst_pipe_fd = open(FIFO_INST, O_RDONLY);
    if (inst_pipe_fd == -1) {
        fprintf(stderr, "Open error on named pipe: %s\n", FIFO_INST);
        exit(EXIT_FAILURE);
    }

    while (true) {
        pid_t pid = fork();

        if (pid > 0) {
            // parent

            // get some instructions to stop.
            if (read(inst_pipe_fd, inst_buf, sizeof(int)) > 0) {
                int inst = *((int *)inst_buf);
                std::cout << "(times:"<< times << ") Parent (pid:" << getpid() << "): get instruction " 
                    << ( inst == EXIT_INST ? "EXIT_INST" : "CONTINURE_INST" )
                    << " to the stdin of child." << std::endl << std::endl;
                if (inst == EXIT_INST) {
                    exit(EXIT_SUCCESS);
                }
            }

        } else {
            // child

            // get test input file path (stored in `buf`) from data pipe.
            if (read(data_pipe_fd, buf, PIPE_BUF_SIZE) > 0) {
                // pipe test input file to `stdin`.
                freopen(buf, "r", stdin);
                std::cout << "(times:"<< times << ") Child (pid:" << getpid() << "): pipe " << buf << " to the stdin of child." << std::endl << std::endl;
            } else {
                fprintf(stderr, "Read error on pipe %s\n", FIFO_DATA);
                exit(EXIT_FAILURE);
            }

            return ;
        }

        times ++;
    }

    (void)close(inst_pipe_fd);
    (void)close(data_pipe_fd);

    exit(EXIT_SUCCESS);
}

