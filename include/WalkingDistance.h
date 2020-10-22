#ifndef WALKINGDISTANCE_H
#define WALKINGDISTANCE_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace WalkingDistance {

using Cost = uint_fast8_t;
using Index = uint_fast16_t;

constexpr Index TABLE_SIZE = 24964;

extern std::array<Cost, TABLE_SIZE> costs;
extern std::array<std::array<std::vector<Index>, 2>, TABLE_SIZE> edges;
extern std::vector<int> row;
extern std::vector<int> col;
extern int width;
extern int height;

void load(const std::vector<int>& goal, std::string name, int width,
          int height);

int getIndex(const std::vector<int>& grid, bool alongRow = true);

};  // namespace WalkingDistance

#endif  // WALKINGDISTANCE_H