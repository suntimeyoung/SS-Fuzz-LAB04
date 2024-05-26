#include "loop.hpp"

void CreateNamedPipe(const char *fifo_path) {
    if(access(fifo_path, F_OK) == -1) {
        if(mkfifo(fifo_path, 0777) != 0) {
            fprintf(stderr, "Could not create fifo %s\n", fifo_path);
            exit(EXIT_FAILURE);
        }
    }
}

void MakeFileFromSeed(const char *file_name, int seed) {
    std::ofstream test_input(file_name);
    test_input << seed/2+3 << " " << seed/2+2 <<  "\n";
    test_input << seed/2 << " " << seed/2+1 <<  "\n";
    test_input.close();
}

void SendInst(int inst_pipe_fd, int continue_or_wait) {
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
