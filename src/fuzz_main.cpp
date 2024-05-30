#define DEMO
#define VERBOSE

#include <chrono>

#include "loop.hpp"
#include "seed.hpp"

RunInfo RandomRunInfo();
MutOp RandomMutOp();
void MakeExampleInitTestcase(int);

int main() {

#ifdef DEMO
    std::cout << "Preparing 20 init testcases for example program..." << std::endl;
    int num = 20;
    while (num--) {
        usleep(5000);
        MakeExampleInitTestcase(num);
    }
    std::cout << "Finished." << std::endl << std::endl;
#endif

    int tested_instance = 0;

    int pid = fork();

    if (pid > 0) {
        // parent

        int data_pipe_fd = OpenNamedPipe(FIFO_DATA, O_WRONLY);
        int inst_pipe_fd = OpenNamedPipe(FIFO_INST, O_WRONLY);

        std::vector<char *> init_files = GetInitTestList();
        std::vector<Seed> init_seeds;
        SeedPool sp;

        for (char *file: init_files) {
            init_seeds.push_back(sp.NewSeed(file));
        }

        SeedManage sm(init_seeds);
        std::vector<Seed> round_seeds;
        
        while (!sm.Empty()) {

            if (tested_instance == TEST_NUM_MAX) break;
            
            if (tested_instance > 0 && !(tested_instance % TEST_NUM_PER_ROUND) ) {
                SendInst(inst_pipe_fd, WAIT_INST);
#ifdef VERBOSE
                std::cout << "(pid:" << getpid() << ") Fuzz_main: sended WAIT_INST." << std::endl;
#endif
                /** handle runtime information, compute score and push mutated seeds into queue. */

                for (Seed round_seed: round_seeds) {
                    round_seed.UpdateRunInfo(RandomRunInfo());
#ifdef VERBOSE
                    std::cout << "(pid:" << getpid() << ") Fuzz_main: round seed score is " << round_seed.Score() << std::endl;
#endif

                    if (round_seed.Score() > SCORE_THRESHOLD) {
                        for (int i = 0; i < MUT_TIME_PER_SEED; i ++) {

                            Seed mut_seed = sp.Mutate(round_seed, RandomMutOp());
                            sm.Push(mut_seed);

                        }
                    }

                }

                sm.Trim();
                round_seeds.clear();

#ifdef VERBOSE
                std::cout << "(pid:" << getpid() << ") Fuzz_main: finished round handling, " << tested_instance << " testcases tested." << std::endl;
#endif
            }

            SendInst(inst_pipe_fd, CONTINUE_INST);

#ifdef VERBOSE
            std::cout << "(pid:" << getpid() << ") Fuzz_main: sended CONTINUE_INST." << std::endl;
#endif

            round_seeds.push_back(sm.Pop());
            std::cout << "(pid:" << getpid() << ") Fuzz_main: testing <" << round_seeds.back().Testcase() << ">..." << std::endl;

            // send the test input file to data pipe.
            if (write(data_pipe_fd, round_seeds.back().Testcase(), PIPE_BUF_SIZE) == -1) {
                fprintf(stderr, "Write error on pipe %s\n", FIFO_DATA);
                exit(EXIT_FAILURE);
            }

            tested_instance ++;

        }

        SendInst(inst_pipe_fd, EXIT_INST);

#ifdef VERBOSE
            std::cout << "(pid:" << getpid() << ") Fuzz_main: sended EXIT_INST." << std::endl;
#endif

        (void)close(inst_pipe_fd);
        (void)close(data_pipe_fd);

        for (char *file: init_files) {
            if (!file) {
                free(file);
            }
        }

    } else if (pid == 0) {
        // child

        // execute tested program.
        execl(TEST_PATH, TEST_PATH, NULL);

        fprintf(stderr, "Could not execl program %s\n", TEST_PATH);
        exit(EXIT_FAILURE);
    }

}

RunInfo RandomRunInfo() {

    usleep(5000);
    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::uniform_int_distribution<uint32_t> dist;
    std::mt19937_64 rng(now);

    /** score in [0.25, 2]. */
    return RunInfo{
        (dist(rng) % 800 + 400) * 1.0 / 100,
        dist(rng) % 350 + 50,
        dist(rng) % 20 + 5
    };

}

MutOp RandomMutOp() {
    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::uniform_int_distribution<uint32_t> dist;
    std::mt19937_64 rng(now);

    return MutOp(dist(rng) % 6);
}

void MakeExampleInitTestcase(int file_no) {
    char file_path[PIPE_BUF_SIZE + 1];
    snprintf(file_path, PIPE_BUF_SIZE, "%sfile_%08x", TEST_DIR, file_no);

    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::uniform_int_distribution<uint32_t> dist;
    std::mt19937_64 rng(now);

    std::ofstream example(file_path);
    example << dist(rng) % 150 + 53  << " " << dist(rng) % 150 + 54 << "\n";
    example << dist(rng) % 150 + 52  << " " << dist(rng) % 150 + 51 << "\n";

    example.close();

}
