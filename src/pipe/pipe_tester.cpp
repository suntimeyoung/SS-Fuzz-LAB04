#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include <iostream>

#define FIFO_FILE "/tmp/fuzz_fifo"
#define BUFFER_SIZE 1024
#define TEST_INSTANCE 10

int main() {
    int pipe_fd;
    int tested_instance = 0;

    if(access(FIFO_FILE, F_OK) == -1)
    {
        if(mkfifo(FIFO_FILE, 0777) != 0)
        {
            fprintf(stderr, "Could not create fifo %s\n", FIFO_FILE);
            exit(EXIT_FAILURE);
        }
    }

    if ((pipe_fd = open(FIFO_FILE, O_WRONLY)) != -1) {
        while (tested_instance++ < TEST_INSTANCE) {
            if (write(pipe_fd, "1 2\n3 4\n", BUFFER_SIZE) == -1) {
                fprintf(stderr, "Write error on pipe\n");
                exit(EXIT_FAILURE);
            }
            // sleep(10);
        }
        (void)close(pipe_fd);
    } else {
        std::cout << "HERE" << std::endl;
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);

}
