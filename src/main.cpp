#include "loop.hpp"

int main() {
    int pid = fork();

    if (pid > 0) {
        // parent

        int tested_instance = 0;

        CreateNamedPipe(FIFO_INST);
        CreateNamedPipe(FIFO_DATA);

        std::cout << "HERE" << std::endl;

        int data_pipe_fd = open(FIFO_DATA, O_WRONLY);
        if (data_pipe_fd == -1) {
            fprintf(stderr, "Open error on named pipe: %s\n", FIFO_DATA);
            exit(EXIT_FAILURE);
        }

        std::cout << "HERE" << std::endl;

        int inst_pipe_fd = open(FIFO_INST, O_WRONLY);
        if (inst_pipe_fd == -1) {
            fprintf(stderr, "Open error on named pipe: %s\n", FIFO_INST);
            exit(EXIT_FAILURE);
        }

        while (tested_instance < TEST_INSTANCE_NUM) {

            // prepare test input file and store its path in `buf` (to be sent to data pipe).
            char buf[PIPE_BUF_SIZE + 1];
            snprintf(buf, PIPE_BUF_SIZE, "%sfile_%d", TEST_DIR, tested_instance++);

            // simulate getting some test input from seed.
            MakeFileFromSeed(buf, tested_instance * tested_instance + 100 * tested_instance);
            std::cout << "\nloop: make test input file stored in: " << buf << std::endl;
            SendInst(inst_pipe_fd, tested_instance == TEST_INSTANCE_NUM ? EXIT_INST : CONTINUE_INST);

            // send the test input file to data pipe.
            if (write(data_pipe_fd, buf, PIPE_BUF_SIZE) == -1) {
                fprintf(stderr, "Write error on pipe %s\n", FIFO_DATA);
                exit(EXIT_FAILURE);
            }

        }


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