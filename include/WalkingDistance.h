#ifndef WALKINGDISTANCE_H
#define WALKINGDISTANCE_H

#include <array>

namespace WalkingDistance {

constexpr auto TABLE_SIZE = 24964;

extern std::array<int, TABLE_SIZE> costs;
extern std::array<std::array<std::array<int, 4>, 2>, TABLE_SIZE> edges;
extern std::array<int, 16> row;
extern std::array<int, 16> col;

void load(const std::array<int, 16>& goal);

int getIndex(const std::array<int, 16>& grid, bool alongRow = true);

};  // namespace WalkingDistance

#endif  // WALKINGDISTANCE_H