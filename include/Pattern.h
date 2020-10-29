#ifndef PATTERN_H
#define PATTERN_H

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "Direction.h"

// Minimal state info
struct Pattern {
    std::vector<int> pos;  // Value to position mapping
    uint64_t id;           // distMap key
    uint64_t g;            // Position to value mapping
};

// Stores pre-computed values for family of Patterns
class PatternGroup {
    const int WIDTH;
    const int HEIGHT;

    std::vector<int> deltas;

public:
    PatternGroup(const std::vector<int>& grid, int width, int height);

    std::vector<int> tiles;
    Pattern initPattern;  // Initial pattern

    int getCell(const Pattern& pattern, int position) const;
    void setCell(Pattern& pattern, int position, int tile);
    bool canShift(const Pattern& pattern, int tile, Direction dir) const;
    Pattern shiftCell(Pattern next, int tile, Direction dir);
};

#endif  // PATTERN_H