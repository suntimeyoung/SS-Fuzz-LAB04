#include <iostream>
#include "seed.hpp"

int main () {

    char file_1[10] = "hello1";
    char file_2[10] = "hello2";
    char file_3[10] = "hello3";
    Seed s_1(file_1);
    Seed s_2(file_2);
    Seed s_3(file_3);
    SeedManage sm;

    s_1._score = 100;
    s_2._score = 90;
    s_3._score = 200;

    sm.Push(s_1);

    std::cout << "First time: " << std::endl;
    for (auto elem: sm._seed_queue) {
        std::cout << elem._score << " ";
    }
    std::cout << std::endl;

    sm.Push(s_2);

    std::cout << "Second time: " << std::endl;
    for (auto elem: sm._seed_queue) {
        std::cout << elem._score << " ";
    }
    std::cout << std::endl;

    sm.Push(s_3);

    std::cout << "Third time: " << std::endl;
    for (auto elem: sm._seed_queue) {
        std::cout << elem._score << " ";
    }
    std::cout << std::endl;

    Seed pop_1 = sm.Pop();
    Seed pop_2 = sm.Pop();
    Seed pop_3 = sm.Pop();

    std::cout << "Pop result: " << std::endl;
    std::cout << pop_1._score << " " << pop_2._score << " " << pop_3._score << std::endl;

}

