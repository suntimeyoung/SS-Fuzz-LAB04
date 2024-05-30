#include "../src/loop.hpp"
#include <set>

unsigned long long last_add = 0;
std::set<unsigned long long> branches;

extern "C" {
    __thread FSHM_TYPE *__cfl_area_ptr;
    __thread int __cfl_prev_loc;
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

    int data_pipe_fd = OpenNamedPipe(FIFO_DATA, O_RDONLY);
    int inst_pipe_fd = OpenNamedPipe(FIFO_INST, O_RDONLY);

    /**  
     * Create a SHM with
     * TEST_INSTANCE_NUM rows * (1 << FSHM_MAX_ITEM_POW2) columns 
     * of type FSHM_TYPE
     **/
    int shmid;
    if ((shmid = shmget(
            (key_t)FSHM_KEY, 
            sizeof(FSHM_TYPE)*(1 << FSHM_MAX_ITEM_POW2) * TEST_NUM_PER_ROUND, 
            0666|IPC_CREAT)) == -1) {
        fprintf(stderr, "Could not create shared memory with key %x\n", FSHM_KEY);
        exit(EXIT_FAILURE);
    }

    /* Attach the SHM to its own memory */
    __cfl_area_ptr = (FSHM_TYPE *) shmat(shmid, NULL, 0);
    __cfl_prev_loc = 0;
    
    while (true) {
        // get some instructions to stop.
        ReceiveInst(inst_pipe_fd);

        pid_t pid = fork();

        if (pid > 0) {
            // parent

            /** send child pid to parent. */
            /* some code. */

        } else {
            // child
            __cfl_area_ptr += (times % TEST_NUM_PER_ROUND) * (1 << FSHM_MAX_ITEM_POW2);
            // std::cout << "__cfl_area_ptr: " << __cfl_area_ptr << std::endl;
            // get test input file path (stored in `buf`) from data pipe.
            if (read(data_pipe_fd, buf, PIPE_BUF_SIZE) > 0) {
                // pipe test input file to `stdin`.
                freopen(buf, "r", stdin);
                std::cout << "(pid:"<< getpid() << ") Child: pipe " << buf << " to the stdin of child." << std::endl;
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

