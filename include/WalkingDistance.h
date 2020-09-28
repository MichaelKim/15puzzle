#ifndef WALKINGDISTANCE_H
#define WALKINGDISTANCE_H

#include <array>
#include <cstdint>

namespace WalkingDistance {

using Cost = uint_fast8_t;
using Index = uint_fast16_t;

constexpr Index TABLE_SIZE = 24964;

extern std::array<Cost, TABLE_SIZE> costs;
extern std::array<std::array<std::array<Index, 4>, 2>, TABLE_SIZE> edges;
extern std::array<int, 16> row;
extern std::array<int, 16> col;

void load(const std::array<int, 16>& goal);

int getIndex(const std::array<int, 16>& grid, bool alongRow = true);

};  // namespace WalkingDistance

#endif  // WALKINGDISTANCE_H