#include "seed.hpp"
#include "loop.hpp"

using namespace std;

/** 
 * 
 * class `Seed`: A unit to run a testcase, get and handle runtime info, perform mutation;
 * 
*/

Seed::Seed(char *base_test, uint32_t seed_hash) {
    _seed_hash = seed_hash;
    snprintf(_base_test, TEST_NAME_LEN, "%s", base_test);
    snprintf(_mut_test, TEST_NAME_LEN, "");
    _score = 0.0;
    _minfo = MutInfo{0, 0, 0, 0, 0, 0};
}

void Seed::UpdateRunInfo(RunInfo rinfo) {
    _rinfo = rinfo;
    ComputeScore();
}

void Seed::ComputeScore() {
    _score = 1.0 * _rinfo.coverage / (_rinfo.size * _rinfo.runtime);
}

// Perform mutation on the `_base_test` according to `minfo`, and saved to `_mut_test`.
void Seed::Mutation(MutOp flag) {

    if (strcmp(_mut_test, "")) {
        fprintf(stderr, "Seed %d already mutated.\n", _seed_hash);
        exit(EXIT_FAILURE);
    }

    switch (flag)
    {
    case BITFLIP:
        /* code */
        break;

    case ARITHMETIC:
        /* code */
        break;

    case INTEREST:
        /* code */
        break;

    case DICTIONARY:
        /* code */
        break;

    case HAVOC:
        /* code */
        break;

    case SPLICE:
        /* code */
        break;
    
    default:
        break;
    }

    snprintf(_mut_test, TEST_NAME_LEN, "%sfile_%04x", TEST_DIR, _seed_hash);

}

/** 
 * 
 * class `SeedPool`: to backup the hash of all created `Seed` and arrange new `Seed`.
 * 
*/

SeedPool::SeedPool() {
    uniform_int_distribution<uint32_t> dist;
    mt19937_64 rng;
    set<uint32_t> pool;
    _dist = dist;
    _rng = rng;
    _pool = pool;
}

Seed SeedPool::NewSeed(char *base_test) {
    uint32_t hash = _dist(_rng);
    while (_pool.count(hash)) {
        hash = _dist(_rng);
    }
    _pool.insert(hash);

    return Seed(base_test, hash);
}

Seed SeedPool::NewSeed(Seed base_seed) {
    uint32_t hash = _dist(_rng);
    while (_pool.count(hash)) {
        hash = _dist(_rng);
    }
    _pool.insert(hash);

    Seed new_seed(base_seed._mut_test, hash);
    new_seed._minfo = base_seed._minfo;

    return new_seed;
}

void SeedPool::Empty() {
    _pool.clear();
}

/** 
 * 
 * class `SeedManage`: to maintain a queue for the untested `Seed` by the order of their score.
 * 
*/

SeedManage::SeedManage(Seed init_s) {
    _seed_queue = vector<Seed>({init_s});
    _qlen = 1;
}

SeedManage::SeedManage(vector<Seed> init_s_vec) {
    _seed_queue = init_s_vec;
    _qlen = init_s_vec.size();

    sort(_seed_queue.begin(), _seed_queue.end(), greater<Seed>());
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

    while (qlen-- > target) {
        _seed_queue.pop_back();
    }

    _qlen = target;
}

size_t SeedManage::Len() {
    return _qlen;
}

void SeedManage::Empty() {
    _seed_queue.clear();
    _qlen = 0;
}
