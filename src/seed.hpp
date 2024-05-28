#pragma once

#include <bits/stdc++.h> 
#include <vector>


#define MAX_QLEN 100;

/** Mutation Operation Flag */
#define BITFLIP 0
#define ARITHMETIC 1
#define INTEREST 2
#define DICTIONARY 3
#define HAVOC 4
#define SPLICE 5


struct RunInfo {
    double runtime;
    uint64_t coverage;
    uint64_t size;
};

class Seed {
public:

    char *_file_name;
    set<uint64_t> _hash;
    double _score;
    RunInfo _info;

    Seed(char* file_name);
    void ComputeScore();
    void Mutation(uint32_t flag);

};

class SeedManage {
public:

    std::vector<Seed> _seed_queue;

    // void 
    void Push(Seed s);
    Seed Pop();

};