#include "loop.hpp"
#include <set>

unsigned long long last_add = 0;
std::set<unsigned long long> branches;

extern "C" {
    FSHM_TYPE *__afl_area_ptr;
    __thread int *__afl_prev_loc;
}

// FSHM_TYPE *__afl_area_ptr;
// __thread int * __afl_prev_loc;
static __thread int prev_loc = 0;

extern "C" void ForkServer() {
    int times = 0;
    char buf[PIPE_BUF_SIZE + 1];
    char inst[PIPE_BUF_SIZE + 1];

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
    
    if ((shmid = shmget((key_t)FSHM_KEY, sizeof(FSHM_TYPE)*(1 << FSHM_MAX_ITEM_POW2) * TEST_INSTANCE_NUM, 0640|IPC_CREAT)) == -1) {
        fprintf(stderr, "Could not create shared memory with key %x\n", FSHM_KEY);
        exit(EXIT_FAILURE);
    }

    /* Attach the SHM to its own memory */
    __afl_area_ptr = (FSHM_TYPE *) shmat(shmid, NULL, 0);

    /* Allocate memory for the prev_loc */
    __afl_prev_loc = &prev_loc;
    

    while (true) {
        pid_t pid = fork();

        if (pid > 0) {
            // parent

            // get some instructions to stop.
            // if (read(inst_pipe_fd, inst, PIPE_BUF_SIZE) > 0) {
            //     if ((int)(*inst) == EXIT_INST) {
            //         exit(EXIT_SUCCESS);
            //     }
            // }
            if (times == TEST_INSTANCE_NUM - 1) {
                exit(EXIT_SUCCESS);
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

