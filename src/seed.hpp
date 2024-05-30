#pragma once

#include <bits/stdc++.h>
#include <sys/stat.h>
#include <vector>
#include <random>
#include <set>


#define MAX_QLEN 100
#define TEST_NAME_LEN 512
#define SCORE_THRESHOLD 0.3
#define MASSIVE_MUT_THRESHOLD 10
#define MUT_TIME_PER_SEED 5

#define TEST_DIR "/tmp/.fuzzlab/testcases/"


const std::vector<uint8_t> interest_vals = {0x00, 0xFF, 0x7F, 0x80};
const std::vector<std::string> dict = {"NULL", "-1", "0", "1"};

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
    double coverage;
    off_t size;
    // RunInfo(double runtime, double coverage, uint32_t size): runtime(runtime), coverage(coverage), size(size) {}
};

struct MutInfo {
    uint32_t bitflip_times;
    uint32_t arithmetic_times;
    uint32_t interest_times;
    uint32_t dictionary_times;
    uint32_t havoc_times;
    uint32_t splice_times;
    uint32_t tot_times;
};


class Seed {
public:

    Seed(char *base_test, uint32_t seed_hash);
    char *Testcase();
    void UpdateRunInfo(RunInfo rinfo);
    void UpdateRunInfo(double runtime, double coverage);
    double Score();
    bool AllowMassive();
    friend bool operator>(const Seed &__a, const Seed &__b) {
        return __a._rank > __b._rank;
    }

    friend class SeedPool;
    friend class SeedManage;

#ifndef SEED_TEST
private:
#endif

    uint32_t _seed_hash;
    char _base_test[TEST_NAME_LEN + 1];
    double _score;
    double _rank;
    RunInfo _rinfo;
    MutInfo _minfo;


};

class SeedPool {
public:

    SeedPool();
    Seed NewSeed(char *base_test);
    Seed Mutate(Seed base_seed, MutOp flag);
    void Clear();

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
    void Clear();
    bool Empty();

#ifndef SEED_TEST
private:
#endif

    std::vector<Seed> _seed_queue;
    size_t _qlen;

};

uint32_t random_uint32(std::mt19937_64 &rng);
uint8_t random_uint8(std::mt19937_64 &rng);
void bitflip(char *data, size_t len, std::mt19937_64 &rng);
void arithmetic(char *data, size_t len, std::mt19937_64 &rng);
void interest(char *data, size_t len, std::mt19937_64 &rng);
void dictionary(char *data, size_t len, std::mt19937_64 &rng);
void havoc(char *data, size_t len, std::mt19937_64 &rng);
void splice(char *data, size_t len, const char *other_data, size_t other_len, std::mt19937_64 &rng);

