#pragma once

#include <bits/stdc++.h> 
#include <vector>
#include <random>
#include <set>


#define MAX_QLEN 100
#define TEST_NAME_LEN 128

/** Mutation Operation Flag */
enum MutOp {
    BITFLIP,
    ARITHMETIC,
    INTEREST,
    DICTIONARY,
    HAVOC,
    SPLICE
};

struct RunInfo {
    double runtime;
    uint32_t coverage;
    uint32_t size;
};

struct MutInfo {
    uint32_t bitflip_times;
    uint32_t arithmetic_times;
    uint32_t interest_times;
    uint32_t dictionary_times;
    uint32_t havoc_times;
    uint32_t splice_times;
};


class Seed {
public:

    Seed(char *base_test, uint32_t seed_hash);
    void UpdateRunInfo(RunInfo rinfo);
    void ComputeScore();
    void Mutation(MutOp flag);
    friend bool operator>(const Seed &__a, const Seed &__b) {
        return __a._score > __b._score;
    }

    friend class SeedPool;
    friend class SeedManage;

#ifndef SEED_TEST
private:
#endif

    uint32_t _seed_hash;
    char _base_test[TEST_NAME_LEN];
    char _mut_test[TEST_NAME_LEN];
    double _score;
    RunInfo _rinfo;
    MutInfo _minfo;


};

class SeedPool {
public:

    SeedPool();
    Seed NewSeed(char *base_test);
    Seed NewSeed(Seed base_seed);
    void Empty();

#ifndef SEED_TEST
private:
#endif

    std::uniform_int_distribution<uint32_t> _dist;
    std::mt19937_64 _rng;
    std::set<uint32_t> _pool;

};

class SeedManage {
public:

    SeedManage(Seed init_s);
    SeedManage(std::vector<Seed> init_s_vec);

    void Push(Seed s);
    Seed Pop();
    void Trim(size_t to_len=0);
    size_t Len();
    void Empty();

#ifndef SEED_TEST
private:
#endif

    std::vector<Seed> _seed_queue;
    size_t _qlen;

};