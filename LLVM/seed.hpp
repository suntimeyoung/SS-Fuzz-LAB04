#include <iostream>
#include <queue>
#include <string>
#include <set>

using namespace std;

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

class SeedManage{
public:
    priority_queue<SeedFile> _seed_queue;
    string absolute_path;

    void push_into_queue(SeedFile sf);
    void _gen_filename();
    void test_queue();
private:
    set<uint32_t> _seed_hash; 
}