#ifndef PATTERN_H
#define PATTERN_H

#include <array>
#include <unordered_map>
#include <vector>

#include "Direction.h"

// Minimal state info
struct Pattern {
    std::array<int, 16> pos;  // Value to position mapping
    uint64_t id;              // distMap key
    uint64_t g;               // Position to value mapping
};

// Stores pre-computed values for family of Patterns
class PatternGroup {
    std::array<int, 16> deltas;
    const int WIDTH = 4;
    const int HEIGHT = 4;

public:
    PatternGroup(const std::array<int, 16>& grid);

    Pattern initPattern;  // Initial pattern

    unsigned getCell(const Pattern& pattern, int position) const;
    void setCell(Pattern& pattern, int position, unsigned tile);
    bool canShift(const Pattern& pattern, unsigned tile, Direction dir) const;
    Pattern shiftCell(Pattern next, unsigned tile, Direction dir);
};

#endif  // PATTERN_H