#include "bitmap.hpp"
#include <cstring>

BitMap::BitMap() {
    memset(_bitmap, 0, sizeof(_bitmap));
    memset(_tmp_bitmap, 0, sizeof(_tmp_bitmap));
    _length  = 1 << FSHM_MAX_ITEM_POW2;
}

double BitMap::compute_score(FSHM_TYPE* new_bitmap) {
    double score = 0.0;
    for (int i=0; i<_length; i++) {
        if (new_bitmap[i] == 0)
            continue;
        if (_bitmap[i] == 0) {
            score += NEW_PATH_WEIGHT;
            _tmp_bitmap[i]++;
        } else {
            score += 1.0 / _bitmap[i];
            _tmp_bitmap[i]++;
        }
    }
    return score;
}

void BitMap::bitmap_update() {
    for (int i=0; i<_length; i++) 
        _bitmap[i] += _tmp_bitmap[i];
    memset(_tmp_bitmap, 0, sizeof(_tmp_bitmap));
}
