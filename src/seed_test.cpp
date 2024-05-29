#define SEED_TEST
#include "seed.hpp"

int main () {

    char file_1[10] = "hello1";
    char file_2[10] = "hello2";
    char file_3[10] = "hello3";

    SeedPool sp;

    Seed s_1 = sp.NewSeed(file_1);
    Seed s_2 = sp.NewSeed(file_2);
    Seed s_3 = sp.NewSeed(file_3);
    
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

    sm.Empty();

    s_1.Mutation(BITFLIP);
    s_2.Mutation(ARITHMETIC);
    s_3.Mutation(INTEREST);

    Seed mut_s_1 = sp.NewSeed(s_1);
    Seed mut_s_2 = sp.NewSeed(s_2);
    Seed mut_s_3 = sp.NewSeed(s_3);

    mut_s_1.UpdateRunInfo(RunInfo{20.0, 210, 60});
    mut_s_2.UpdateRunInfo(RunInfo{3.0, 150, 80});
    mut_s_3.UpdateRunInfo(RunInfo{10.0, 180, 5});

    mut_s_1.Mutation(BITFLIP);
    mut_s_2.Mutation(ARITHMETIC);
    mut_s_3.Mutation(INTEREST);

    sm.Push(s_1);
    sm.Push(s_2);
    sm.Push(s_3);
    sm.Push(mut_s_1);
    sm.Push(mut_s_2);
    sm.Push(mut_s_3);

    for (auto elem: sm._seed_queue) {
        std::cout << "(score=" << elem._score << 
                     ", hash=" << elem._seed_hash << 
                     ", cur_test=" << elem._cur_test << ")" << 
        std::endl;
    }
    std::cout << std::endl;

}

