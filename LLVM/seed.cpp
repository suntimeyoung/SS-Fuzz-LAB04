#include <random>

#include "seed.hpp"

void SeedManage::push_into_queue(SeedFile sf) {
}

void SeedManage::_gen_filename() {
    std::mt19937_64 rng; // 随机数生成器
    std::uniform_int_distribution<uint32_t> dist;
}

void SeedManage::test_queue() {
}
