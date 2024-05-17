#include <iostream>
#include <queue>
#include <string>
#include <set>
#include <vector>

using namespace std;

/*The class of seedfile, including the rules of priority queue*/
class SeedFile{
public:
    string _filename;
    uint64_t _coverage;
    double _runtime;
    uint64_t _size;

    SeedFile(string filename, uint64_t coverage, uint64_t size): 
        _filename(filename), _coverage(coverage), _size(size) {}
    
    friend bool operator > (const SeedFile &a, const SeedFile &b) {
        if (a._coverage != b._coverage) return a._coverage > b._coverage;
        if (a._runtime != b._runtime) return a._runtime < b._runtime;
        return a._size < b._size;
    }
    
    
};

/*The class of seedfile management, including the initialization, the seed derivation, the seed storage, and seed aquirement*/
class SeedManage{
public:
    string absolute_path;
    int mutation_small_count;
    int mutation_large_count;
    
    SeedManage(string _absolute_path, int _mutation_small_count, int _mutation_large_count):
        absolute_path(_absolute_path), mutation_small_count(_mutation_small_count), mutation_large_count(_mutation_large_count) {}
    SeedManage() {}

    vector<SeedFile> seed_init(int init_num);
    vector<SeedFile> seed_mutation_small();
    vector<SeedFile> seed_mutation_large();
    void push_into_queue(SeedFile sf);
    SeedFile get_next_seedfile();
    void test_queue();
private:
    set<uint32_t> _seed_hash; 
    priority_queue<SeedFile> _seed_queue;

    string _gen_filename();
}