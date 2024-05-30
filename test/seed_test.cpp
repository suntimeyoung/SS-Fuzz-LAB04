#define SEED_TEST
#include "seed.hpp"
#include "loop.hpp"

int main () {

    char file_path_1[PIPE_BUF_SIZE + 1];
    char file_path_2[PIPE_BUF_SIZE + 1];
    char file_path_3[PIPE_BUF_SIZE + 1];
    snprintf(file_path_1, PIPE_BUF_SIZE, "%sfile_%08x", TEST_DIR, 1);
    snprintf(file_path_2, PIPE_BUF_SIZE, "%sfile_%08x", TEST_DIR, 2);
    snprintf(file_path_3, PIPE_BUF_SIZE, "%sfile_%08x", TEST_DIR, 3);

    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::uniform_int_distribution<uint32_t> dist;
    std::mt19937_64 rng(now);

    std::ofstream example_1(file_path_1);
    example_1 << dist(rng) % 150 + 53  << " " << dist(rng) % 150 + 54 << "\n";
    example_1 << dist(rng) % 150 + 52  << " " << dist(rng) % 150 + 51 << "\n";
    example_1.close();

    sleep(1);

    std::ofstream example_2(file_path_2);
    example_2 << dist(rng) % 150 + 53  << " " << dist(rng) % 150 + 54 << "\n";
    example_2 << dist(rng) % 150 + 52  << " " << dist(rng) % 150 + 51 << "\n";

    sleep(1);

    std::ofstream example_3(file_path_1);
    example_3 << dist(rng) % 150 + 53  << " " << dist(rng) % 150 + 54 << "\n";
    example_3 << dist(rng) % 150 + 52  << " " << dist(rng) % 150 + 51 << "\n";
    example_3.close();


    SeedPool sp;

    Seed s_1 = sp.NewSeed(file_path_1);
    Seed s_2 = sp.NewSeed(file_path_2);
    Seed s_3 = sp.NewSeed(file_path_3);
    
    s_1.UpdateRunInfo(RunInfo{10.0, 100, 20});
    s_2.UpdateRunInfo(RunInfo{5.0, 60, 40});
    s_3.UpdateRunInfo(RunInfo{3.0, 120, 10});

    SeedManage sm(s_1);

    std::cout << "First time: " << std::endl;
    for (auto elem: sm._seed_queue) {
        std::cout << "(" << elem._score << "," << elem._seed_hash << ")" << " ";
    }
    std::cout << std::endl << std::endl;

    sm.Push(s_2);

    std::cout << "Second time: " << std::endl;
    for (auto elem: sm._seed_queue) {
        std::cout << "(" << elem._score << "," << elem._seed_hash << ")" << " ";
    }
    std::cout << std::endl << std::endl;

    sm.Push(s_3);

    std::cout << "Third time: " << std::endl;
    for (auto elem: sm._seed_queue) {
        std::cout << "(" << elem._score << "," << elem._seed_hash << ")" << " ";
    }
    std::cout << std::endl << std::endl;

    Seed pop_1 = sm.Pop();
    Seed pop_2 = sm.Pop();
    Seed pop_3 = sm.Pop();

    std::cout << "Pop result: " << std::endl;
    std::cout << pop_1._score << " " << pop_2._score << " " << pop_3._score << std::endl;
    std::cout << std::endl;

    uint32_t times = 10;
    while (times--) {
        sm.Push(s_1);
    }

    std::cout << "Length before trimming: " << sm.Len() << std::endl;
    sm.Trim(5);
    std::cout << "Length after trimming: " << sm.Len() << std::endl;
    std::cout << std::endl;

    sm.Clear();

    Seed mut_s_1 = sp.Mutate(s_1, BITFLIP);
    Seed mut_s_2 = sp.Mutate(s_2, ARITHMETIC);
    Seed mut_s_3 = sp.Mutate(s_3, INTEREST);

    mut_s_1.UpdateRunInfo(RunInfo{20.0, 210, 60});
    mut_s_2.UpdateRunInfo(RunInfo{3.0, 150, 80});
    mut_s_3.UpdateRunInfo(RunInfo{10.0, 180, 5});

    Seed mmut_s_1 = sp.Mutate(mut_s_1, BITFLIP);
    Seed mmut_s_2 = sp.Mutate(mut_s_2, ARITHMETIC);
    Seed mmut_s_3 = sp.Mutate(mut_s_3, INTEREST);

    sm.Push(mut_s_1);
    sm.Push(mut_s_2);
    sm.Push(mut_s_3);
    sm.Push(mmut_s_1);
    sm.Push(mmut_s_2);
    sm.Push(mmut_s_3);

    for (auto elem: sm._seed_queue) {
        std::cout << "(score=" << elem._score << 
                     ", hash=" << elem._seed_hash << 
                     ", base_test=" << elem._base_test << ")" << 
        std::endl;
    }
    std::cout << std::endl;

}

