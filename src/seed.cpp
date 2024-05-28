#include "seed.hpp"
#include "loop.hpp"

using namespace std;

Seed::Seed(char *file_name) {
    _file_name = file_name;
}

void Seed::ComputeScore() {
    _score = _info.coverage / (_info.size * _info.runtime);
}

void Seed::Mutation(uint32_t flag) {
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
}

void SeedManage::Push(Seed s) {
    _seed_queue.push_back(s);

    std::sort(_seed_queue.begin(), _seed_queue.end(), [](Seed &__a, Seed &__b) {
        return __a._score > __b._score;
    });
}

Seed SeedManage::Pop() {
    Seed first = *_seed_queue.begin();
    _seed_queue.erase(_seed_queue.begin());

    return first;
}
