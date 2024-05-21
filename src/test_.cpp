#include "loop.hpp"

int FuncA(int);
int FuncB(int);
int FuncC(int, int);
void ForkServer();

int main () {

    ForkServer();

    int a, b;
    int cnt = 2;
    do {
        std::cin >> a >> b;
        if (a > b)
            std::cout << "(pid:" << getpid() << ") Result of FuncC: " << FuncC(a, b) << std::endl;
        std::cout << "(pid:" << getpid() << ") Result of FuncA + FuncB: " << FuncA(a) + FuncB(b) << std::endl;  
    } while (--cnt > 0);

    return 0;
}

int FuncA(int a) {
    return 2*a;
}

int FuncB(int b) {
    return 3*b;
}

int FuncC(int a, int b) {
    if (a * 10 < b) {
        return a + b;
    }
    return a - b;
}

void ForkServer() {
    int times = 0;
    char buf[PIPE_BUF_SIZE + 1];

    int data_pipe_fd = open(FIFO_DATA, O_RDONLY);
    if (data_pipe_fd == -1) {
        fprintf(stderr, "Open error on named pipe: %s\n", FIFO_DATA);
        exit(EXIT_FAILURE);
    }

    while (true) {

        pid_t pid = fork();

        if (pid > 0) {
            // parent

            // simulate getting some instructions to stop.
            if (times == TEST_INSTANCE_NUM - 1) {
                // sleep(10);
                exit(EXIT_SUCCESS);
            }

        } else {
            // child

            // get test input file path (stored in `buf`) from data pipe.
            if (read(data_pipe_fd, buf, PIPE_BUF_SIZE) > 0) {
                // pipe test input file to `stdin`.
                freopen(buf, "r", stdin);
                std::cout << "\n(times:"<< times << ") Child (pid:" << getpid() << "): pipe " << buf << " to the stdin of child." << std::endl;
            } else {
                fprintf(stderr, "Read error on pipe %s\n", FIFO_DATA);
                exit(EXIT_FAILURE);
            }

            return ;
        }

        times ++;
    }

    (void)close(data_pipe_fd);
    exit(EXIT_SUCCESS);
}