#include "loop.hpp"

int main() {
    int pid = fork();

    if (pid > 0) {
        // parent

        int tested_instance = 0;

        std::cout << "here-0" << std::endl;
        CreateNamedPipe(FIFO_INST);
        CreateNamedPipe(FIFO_DATA);

        int data_pipe_fd = open(FIFO_DATA, O_WRONLY);
        if (data_pipe_fd == -1) {
            fprintf(stderr, "Open error on named pipe: %s\n", FIFO_DATA);
            std::cout << "here-1" << std::endl;
            exit(EXIT_FAILURE);
        }

        int inst_pipe_fd = open(FIFO_INST, O_WRONLY);
        if (inst_pipe_fd == -1) {
            fprintf(stderr, "Open error on named pipe: %s\n", FIFO_INST);
            std::cout << "here-2" << std::endl;
            exit(EXIT_FAILURE);
        }
        
        std::cout << "here-3" << std::endl;
        while (tested_instance < TEST_INSTANCE_NUM) {
            std::cout << "here-4" << std::endl;

            // prepare test input file and store its path in `buf` (to be sent to data pipe).
            char buf[PIPE_BUF_SIZE + 1];
            snprintf(buf, PIPE_BUF_SIZE, "%sfile_%d", TEST_DIR, tested_instance++);

            // simulate getting some test input from seed.
            MakeFileFromSeed(buf, tested_instance * tested_instance + 100 * tested_instance);
            std::cout << "\nloop: make test input file stored in: " << buf << std::endl;

            // send the test input file to data pipe.
            if (write(data_pipe_fd, buf, PIPE_BUF_SIZE) == -1) {
                fprintf(stderr, "Write error on pipe %s\n", FIFO_DATA);
                exit(EXIT_FAILURE);
            }

        }

        SendInst(inst_pipe_fd, EXIT_INST);

        (void)close(inst_pipe_fd);
        (void)close(data_pipe_fd);

    } else if (pid == 0) {
        // child

        // execute tested program.
        execl(TEST_PATH, TEST_PATH, NULL);

        fprintf(stderr, "Could not execl program %s\n", TEST_PATH);
        exit(EXIT_FAILURE);
    }

}

void CreateNamedPipe(const char *fifo_path) {
    if(access(fifo_path, F_OK) == -1) {
        if(mkfifo(fifo_path, 0777) != 0) {
            fprintf(stderr, "Could not create fifo %s\n", fifo_path);
            exit(EXIT_FAILURE);
        }
    }
}

int GetOrCreateSharedMem() {
    int shmid;
    
    /* Create a SHM with TEST_INSTANCE_NUM rows and (1 << FSHM_MAX_ITEM_POW2) columns of type FSHM_TYPE */
    if ((shmid = shmget((key_t)FSHM_KEY, sizeof(FSHM_TYPE)*(1 << FSHM_MAX_ITEM_POW2) * TEST_INSTANCE_NUM, 0640|IPC_CREAT)) == -1) {
        fprintf(stderr, "Could not create shared memory with key %x\n", FSHM_KEY);
        exit(EXIT_FAILURE);
    } else {
        return shmid;
    }
}

void ReadSharedMem(char *buf, int buf_size) {

}

void WriteSharedMem(const char *buf, int pos, int offset) {

}

void DeleteSharedMem(int shmid) {

}

void MakeFileFromSeed(const char *file_name, int seed) {
    std::ofstream test_input(file_name);
    test_input << seed/2+3 << " " << seed/2+2 <<  "\n";
    test_input << seed/2 << " " << seed/2+1 <<  "\n";
    test_input.close();
}

void SendInst(const int inst_pipe_fd, const int continue_or_wait) {
    if (inst_pipe_fd != -1) {
        if (write(inst_pipe_fd, &continue_or_wait, sizeof(continue_or_wait)) == -1) {
            fprintf(stderr, "Write error on pipe %s\n", FIFO_INST);
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Open error on named pipe: %s\n", FIFO_DATA);
        exit(EXIT_FAILURE);
    }
}
