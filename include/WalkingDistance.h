#ifndef WALKINGDISTANCE_H
#define WALKINGDISTANCE_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace WalkingDistance {

using Cost = uint_fast8_t;
using Index = uint_fast16_t;

extern std::vector<Cost> costs;
extern std::vector<std::vector<Index>> edgesUp;
extern std::vector<std::vector<Index>> edgesDown;
extern std::vector<int> row;
extern std::vector<int> col;
extern int width;
extern int height;

void load(const std::vector<int>& goal, int width, int height);

int getIndex(const std::vector<int>& grid, bool alongRow = true);

};  // namespace WalkingDistance

#endif  // WALKINGDISTANCE_H