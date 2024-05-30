#include "seed.hpp"
#include "loop.hpp"

using namespace std;

/** 
 * 
 * class `Seed`: A unit to run a testcase, get and handle runtime info, perform mutation;
 * 
 **/

Seed::Seed(char *base_test, uint32_t seed_hash) {
    _seed_hash = seed_hash;
    snprintf(_base_test, TEST_NAME_LEN, "%s", base_test);
    _score = -1.0;
    _rank = -1.0;
    _rinfo = RunInfo{0.0, 0, 0};
    _minfo = MutInfo{0, 0, 0, 0, 0, 0};
}

char *Seed::Testcase() {
    return _base_test;
}

void Seed::UpdateRunInfo(RunInfo rinfo) {
    _rinfo = rinfo;
    _score = 1.0 * _rinfo.coverage / (_rinfo.size * _rinfo.runtime);
}

double Seed::Score() {

    if (_score < 0) {
        if (_rinfo.runtime == 0) {
            fprintf(stderr, "Seed %d has no runtime info yet.", _seed_hash);
            exit(EXIT_FAILURE);
        }
        _score = 1.0 * _rinfo.coverage / (_rinfo.size * _rinfo.runtime);
    }

    return _score;
}

/** 
 * 
 * class `SeedPool`: to backup the hash of all created `Seed` and arrange new `Seed`.
 * 
 **/

SeedPool::SeedPool() {
    uniform_int_distribution<uint32_t> dist;
    mt19937_64 rng;
    set<uint32_t> pool;
    _dist = dist;
    _rng = rng;
    _pool = pool;
}

// create a new seed directly from init testcase(with no `_score` and infinite `_rank`).
Seed SeedPool::NewSeed(char *base_test) {
    uint32_t hash = _dist(_rng);
    while (_pool.count(hash)) {
        hash = _dist(_rng);
    }
    _pool.insert(hash);

    return Seed(base_test, hash);
}

// perform mutation on the `base_seed` according to `flag`, and return mutated seed.
Seed SeedPool::Mutate(Seed base_seed, MutOp flag) {

    uint32_t hash = _dist(_rng);
    while (_pool.count(hash)) {
        hash = _dist(_rng);
    }
    _pool.insert(hash);

    char base_test[TEST_NAME_LEN + 1];
    snprintf(base_test, TEST_NAME_LEN, "%sfile_%08x", TEST_DIR, hash);

    Seed mut_seed(base_test, hash);
    mut_seed._minfo = base_seed._minfo;
    mut_seed._rank = base_seed._score;

    switch (flag)
    {
    case BITFLIP:
        mut_seed._minfo.bitflip_times ++;
        /* code */
        break;

    case ARITHMETIC:
        mut_seed._minfo.arithmetic_times ++;
        /* code */
        break;

    case INTEREST:
        mut_seed._minfo.interest_times ++;
        /* code */
        break;

    case DICTIONARY:
        mut_seed._minfo.dictionary_times ++;
        /* code */
        break;

    case HAVOC:
        mut_seed._minfo.havoc_times ++;
        /* code */
        break;

    case SPLICE:
        mut_seed._minfo.splice_times ++;
        /* code */
        break;
    
    default:
        fprintf(stderr, "Bad Mutation Flag.");
        exit(EXIT_FAILURE);
    }

    // temporarily copy the old testcase to the new one.
    std::ifstream base(base_seed._base_test);
    std::ofstream mut(mut_seed._base_test);
    mut << base.rdbuf();

    return mut_seed;
}

void SeedPool::Clear() {
    _pool.clear();
}

/** 
 * 
 * class `SeedManage`: to maintain a queue for the untested `Seed` by the order of their score.
 * 
 **/

SeedManage::SeedManage(Seed init_s) {
    init_s._rank = DBL_MAX;
    _seed_queue = vector<Seed>({init_s});
    _qlen = 1;
}

SeedManage::SeedManage(vector<Seed> init_s_vec) {
    _seed_queue = init_s_vec;
    _qlen = 0;

    for (Seed s: _seed_queue) {
        s._rank = DBL_MAX;
        _qlen ++;
    }
}

void SeedManage::Push(Seed s) {
    _seed_queue.push_back(s);
    _qlen ++;

    sort(_seed_queue.begin(), _seed_queue.end(), greater<Seed>());
}

Seed SeedManage::Pop() {
    Seed first = *_seed_queue.begin();
    _seed_queue.erase(_seed_queue.begin());
    _qlen --;

    return first;
}

void SeedManage::Trim(size_t to_len) {
    size_t qlen = _qlen;
    size_t target = to_len == 0 ? MAX_QLEN : to_len;

    if (qlen <= target) return;

    while (qlen-- > target) {
        _seed_queue.pop_back();
    }

    _qlen = target;
}

size_t SeedManage::Len() {
    return _qlen;
}

void SeedManage::Clear() {
    _seed_queue.clear();
    _qlen = 0;
}

bool SeedManage::Empty() {
    return _qlen == 0;
}
