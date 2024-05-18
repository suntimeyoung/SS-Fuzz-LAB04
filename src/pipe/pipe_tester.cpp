#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>

#define FIFO_DATA "/tmp/fuzz_fifo_data"
#define FIFO_INST "/tmp/fuzz_fifo_inst"
#define BUFFER_SIZE 1024
#define TEST_INSTANCE_NUM 20
#define CONTINUE_INST 0
#define WAIT_INST 1

void makeFileFromSeed(char *file_name, int seed) {
    std::ofstream test_input(file_name);
    test_input << seed/2+3 << " " << seed/2+2 <<  "\n";
    test_input << seed/2 << " " << seed/2+1 <<  "\n";
    test_input.close();
}

void sendInst(int continue_or_wait) {
    int inst_pipe_fd = open(FIFO_INST, O_WRONLY);
    if (inst_pipe_fd != -1) {
        if (write(inst_pipe_fd, &continue_or_wait, sizeof(continue_or_wait)) == -1) {
            fprintf(stderr, "Write error on pipe %s\n", FIFO_INST);
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Open error on named pipe: %s\n", FIFO_DATA);
        exit(EXIT_FAILURE);
    }
    (void)close(inst_pipe_fd);
}

int main() {
    int data_pipe_fd;
    int tested_instance = 0;

    if(access(FIFO_INST, F_OK) == -1)
    {
        if(mkfifo(FIFO_INST, 0777) != 0)
        {
            fprintf(stderr, "Could not create fifo %s\n", FIFO_INST);
            exit(EXIT_FAILURE);
        }
    }

    if(access(FIFO_DATA, F_OK) == -1)
    {
        if(mkfifo(FIFO_DATA, 0777) != 0)
        {
            fprintf(stderr, "Could not create fifo %s\n", FIFO_DATA);
            exit(EXIT_FAILURE);
        }
    }

    data_pipe_fd = open(FIFO_DATA, O_WRONLY);
    if (data_pipe_fd != -1) {
        while (tested_instance < TEST_INSTANCE_NUM) {

            // prepare test input file and store its path in `buffer` (to be sent to data pipe).
            char buffer[BUFFER_SIZE + 1];
            snprintf(buffer, BUFFER_SIZE, "/tmp/input_file_path_%d", tested_instance++);
            std::cout << "pipe_tester: make test input file stored in: " << buffer << std::endl;

            // simulate getting some test input from seed.
            makeFileFromSeed(buffer, tested_instance * tested_instance + 100 * tested_instance);

            // send the test input file to data pipe.
            if (write(data_pipe_fd, buffer, BUFFER_SIZE) == -1) {
                fprintf(stderr, "Write error on pipe %s\n", FIFO_DATA);
                exit(EXIT_FAILURE);
            }

        }

        (void)close(data_pipe_fd);

    } else {
        fprintf(stderr, "Open error on named pipe: %s\n", FIFO_DATA);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);

}
