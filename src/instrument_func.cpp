#include "loop.hpp"
#include <set>

unsigned long long last_add = 0;
std::set<unsigned long long> branches;

extern "C" {
    FSHM_TYPE *__afl_area_ptr;
    __thread int __afl_prev_loc;
    // void ForkServer();
}

extern "C" void UpdateCoverageMap(int32_t Incr, int32_t* MapPtrIdx) {
    std::cout << "Incr value: " << Incr << std::endl;
    std::cout << "MapPtrIdx address: " << MapPtrIdx << std::endl;
    std::cout << "Value at MapPtrIdx: " << *MapPtrIdx << std::endl;
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

    /* Create a SHM with TEST_INSTANCE_NUM rows * (1 << FSHM_MAX_ITEM_POW2) columns of type FSHM_TYPE */
    int shmid;
    if ((shmid = shmget((key_t)FSHM_KEY, sizeof(FSHM_TYPE)*(1 << FSHM_MAX_ITEM_POW2) * TEST_INSTANCE_NUM, 0666|IPC_CREAT)) == -1) {
        fprintf(stderr, "Could not create shared memory with key %x\n", FSHM_KEY);
        exit(EXIT_FAILURE);
    }

    /* Attach the SHM to its own memory */
    __afl_area_ptr = (FSHM_TYPE *) shmat(shmid, NULL, 0);
    __afl_prev_loc = 0;
    
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

