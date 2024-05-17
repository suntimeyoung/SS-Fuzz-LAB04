#include <random>

#include "seed.hpp"

/*The initialization of seedfiles, generated accroding to the data model*/
vector<SeedFile> SeedManage::seed_init(int init_num) {
    /*  
        generates the inital seedfiles, stores them on disk, and returns the SeedFile of the files
        not adding the files in to the priority queue 
    */
    return vector<SeedFile>();
}

vector<SeedFile> SeedManage::seed_mutation_small() {
    /*
        takes the top of the priority queue and mutates it slightly, stores them on the disk, and returns the SeedFile of the filse
        not adding the files in to the priority queue 
    */
    return vector<SeedFile>();
}

vector<SeedFile> SeedManage::seed_mutation_large() {
    /*
        takes the top of the priority queue and mutates it largely, stores them on the disk, and returns the SeedFile of the filse
        not adding the files in to the priority queue 
    */
    return vector<SeedFile>();
}

/*The name of the seedfile will be modified automatically*/
void SeedManage::push_into_queue(SeedFile sf) {
    sf._filename = _gen_filename();
    _seed_queue.push(sf);
}

SeedFile SeedManage::get_next_seedfile() {
    SeedFile top_sf = _seed_queue.top();
    _seed_queue.pop()
    return top_sf;
}

string SeedManage::_gen_filename() {
    std::mt19937_64 rng; // 随机数生成器
    std::uniform_int_distribution<uint32_t> dist;
    string filename = "seed-";
    uint32_t random_value;
    do {
        random_value = dist(rng);
    } while (_seed_hash.count(random_value));
    return filename + to_string(random_value);
}

void SeedManage::test_queue() {
}
