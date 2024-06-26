#define DEMO
#define VERBOSE

#include <chrono>

#include "loop.hpp"
#include "seed.hpp"
#include "bitmap.hpp"


void MakeNormalInitTestcase(int);
void MakeAbnormalInitTestcase(int);

int main(int argc, char **argv) {

    char test_prog[PIPE_BUF_SIZE + 1];
    char test_log[PIPE_BUF_SIZE + 1];

    if (argc > 2) {
        fprintf(stderr, "Bad arguments, using format: bin/fuzz_main [test_prog_name].");
        exit(EXIT_FAILURE);
    } else if (argc == 1) {
        snprintf(test_prog, PIPE_BUF_SIZE, "%s%s", TEST_PROG_DIR, DEFAULT_TEST_PROG);
        snprintf(test_log, PIPE_BUF_SIZE, "%s%s.log", LOG_DIR, DEFAULT_TEST_PROG);
    } else {
        snprintf(test_prog, PIPE_BUF_SIZE, "%s%s", TEST_PROG_DIR, argv[1]);
        snprintf(test_log, PIPE_BUF_SIZE, "%s%s.log", LOG_DIR, argv[1]);
    }

    std::ofstream log(test_log);

#ifdef DEMO
    log << "Preparing 20 init testcases for example program..." << std::endl;
    int num = 20;
    while (num-- > 10) {
        usleep(5000);
        MakeNormalInitTestcase(num);
    }
    while (num--) {
        usleep(5000);
        MakeAbnormalInitTestcase(num);
    }
    log << "Finished." << std::endl << std::endl;
#endif

    std::uniform_int_distribution<uint8_t> dist(0, UINT8_MAX);
    std::mt19937_64 rng;
    int tested_instance = 0;

    int pid = fork();

    if (pid > 0) {
        // parent

        int data_pipe_fd = OpenNamedPipe(FIFO_DATA, O_WRONLY);
        int inst_pipe_fd = OpenNamedPipe(FIFO_INST, O_WRONLY);
        int shmid_bitmap = GetOrCreateSharedMem(FSHM_KEY, sizeof(FSHM_TYPE)*(1 << FSHM_MAX_ITEM_POW2) * TEST_NUM_PER_ROUND);
        int shmid_time = GetOrCreateSharedMem(FSHM_TIME_KEY, sizeof(PidTime) * TEST_NUM_PER_ROUND);

        FSHM_TYPE *__cfl_area_ptr = (FSHM_TYPE *) shmat(shmid_bitmap, NULL, 0);
        PidTime *__cfl_time_ptr = (PidTime *) shmat(shmid_time, NULL, 0);
        memset(__cfl_time_ptr, 0, sizeof(PidTime) * TEST_NUM_PER_ROUND);

        std::vector<char *> init_files = GetInitTestList();
        std::vector<Seed> init_seeds;
        SeedPool sp;

        for (char *file: init_files) {
            init_seeds.push_back(sp.NewSeed(file));
        }

        SeedManage sm(init_seeds);
        std::vector<Seed> round_seeds;
        BitMap bitmap;
        
        while (!sm.Empty()) {
            
            if (tested_instance > 0 && !(tested_instance % TEST_NUM_PER_ROUND) ) {
                SendInst(inst_pipe_fd, WAIT_INST);
                while (!ProgFinish(__cfl_time_ptr));

#ifdef VERBOSE
                log << "(pid:" << getpid() << ") Fuzz_main: sended WAIT_INST." << std::endl;
#endif

                /** handle runtime information, compute score and push mutated seeds into queue. */

                for (int i = 0; i < TEST_NUM_PER_ROUND; i ++) {
                    round_seeds[i].UpdateRunInfo(
                        1.0 * __cfl_time_ptr[i].duration_time / CLOCKS_PER_SEC,
                        bitmap.ComputeScore(__cfl_area_ptr + i * (1 << FSHM_MAX_ITEM_POW2))
                    );
                    
#ifdef VERBOSE
                    log << "(pid:" << getpid() << ") Fuzz_main: round seed score is " << round_seeds[i].Score() << std::endl;
#endif

                    if (round_seeds[i].Score() > SCORE_THRESHOLD) {
                        for (int i = 0; i < MUT_TIME_PER_SEED; i ++) {

                            if (round_seeds[i].AllowMassive()) {
                                Seed mut_seed = sp.Mutate(round_seeds[i], MutOp(dist(rng) % SPLICE));
                                sm.Push(mut_seed);
                            } else {
                                Seed mut_seed = sp.Mutate(round_seeds[i], MutOp(dist(rng) % DICTIONARY));
                                sm.Push(mut_seed);
                            }

                        }
                    }

                }

                sm.Trim();
                round_seeds.clear();
                bitmap.Update();
                memset(__cfl_time_ptr, 0, sizeof(PidTime) * TEST_NUM_PER_ROUND);

#ifdef VERBOSE
                log << "(pid:" << getpid() << ") Fuzz_main: finished round handling, " << tested_instance << " testcases tested." << std::endl;
#endif

            if (tested_instance == TEST_NUM_MAX) break;

            }

            SendInst(inst_pipe_fd, CONTINUE_INST);

#ifdef VERBOSE
            log << "(pid:" << getpid() << ") Fuzz_main: sended CONTINUE_INST." << std::endl;
#endif

            round_seeds.push_back(sm.Pop());
            log << "(pid:" << getpid() << ") Fuzz_main: testing <" << round_seeds.back().Testcase() << ">..." << std::endl;

            // send the test input file to data pipe.
            char buf[2*PIPE_BUF_SIZE];
            memcpy(buf, round_seeds.back().Testcase(), PIPE_BUF_SIZE);
            snprintf(buf+PIPE_BUF_SIZE, PIPE_BUF_SIZE, "%d", tested_instance % TEST_NUM_PER_ROUND);
            // log << buf << std::endl;
            if (write(data_pipe_fd, buf, 2*PIPE_BUF_SIZE) == -1) {
                fprintf(stderr, "Write error on pipe %s\n", FIFO_DATA);
                exit(EXIT_FAILURE);
            }

            tested_instance ++;

        }

        SendInst(inst_pipe_fd, EXIT_INST);

#ifdef VERBOSE
            log << "(pid:" << getpid() << ") Fuzz_main: finished all rounds, " << tested_instance << " testcases tested." << std::endl;
            log << "(pid:" << getpid() << ") Fuzz_main: sended EXIT_INST." << std::endl;
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
        execl(test_prog, test_prog, NULL);

        fprintf(stderr, "Could not execl program %s\n", test_prog);
        exit(EXIT_FAILURE);
    }

}

void MakeNormalInitTestcase(int file_no) {
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

void MakeAbnormalInitTestcase(int file_no) {
    char file_path[PIPE_BUF_SIZE + 1];
    snprintf(file_path, PIPE_BUF_SIZE, "%sfile_%08x", TEST_DIR, file_no);

    auto& chrs = "-+0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::uniform_int_distribution<std::string::size_type> dist(0, sizeof(chrs) - 2);
    std::mt19937 rng{std::random_device{}()};

    std::ofstream example(file_path);
    std::string buf;
    int length;

    length = dist(rng) + 20;
    buf.reserve(length);
    while (length--) {
        buf += chrs[dist(rng)];
    }
    example << buf << "\n";

    buf.clear();

    length = dist(rng) + 20;
    buf.reserve(length);
    while (length--) {
        buf += chrs[dist(rng)];
    }
    example << buf << "\n";

    example.close();

}
