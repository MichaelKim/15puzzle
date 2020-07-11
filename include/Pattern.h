#ifndef PATTERN_H
#define PATTERN_H

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
    std::vector<int> deltas;
    const int WIDTH;
    const int HEIGHT;

public:
    PatternGroup(const std::vector<std::vector<int>>& grid, const int WIDTH,
                 const int HEIGHT);

    Pattern initPattern;  // Initial pattern

    int getCell(const Pattern& pattern, int position) const;
    void setCell(Pattern& pattern, int position, int tile);
    bool canShift(const Pattern& pattern, int tile, Direction dir) const;
    Pattern shiftCell(Pattern next, int tile, Direction dir);
};

#endif  // PATTERN_H