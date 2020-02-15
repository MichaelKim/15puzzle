#ifndef PATTERN_H
#define PATTERN_H

#include "Direction.h"

#include <cstdint>
#include <vector>

typedef unsigned int uint;

struct Pattern {
    uint64_t id;           // distMap key
    uint dist;             // distMap value
    std::vector<int> pos;  // Value to position mapping
    uint64_t g;            // Position to value mapping

    const uint WIDTH, HEIGHT;

    int getCell(int posn) const;
    void setCell(int posn, int tile);
    bool canShift(int tile, Direction dir) const;
    Pattern shiftCell(int tile, Direction dir,
                      const std::vector<int>& deltas) const;
};

#endif  // PATTERN_H
