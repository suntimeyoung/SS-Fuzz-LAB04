#pragma once

#include <stdint.h>

#define FSHM_MAX_ITEM_POW2 16
#define FSHM_TYPE unsigned int
#define NEW_PATH_WEIGHT 2

class BitMap {
public:
    BitMap();
    double ComputeScore(FSHM_TYPE *new_bitmap);
    void Update();

private:
    uint32_t _length;
    FSHM_TYPE _bitmap[1 << FSHM_MAX_ITEM_POW2];
    FSHM_TYPE _tmp_bitmap[1 << FSHM_MAX_ITEM_POW2];
};

