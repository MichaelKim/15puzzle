#ifndef WALKINGDISTANCE_H
#define WALKINGDISTANCE_H

#include <array>

namespace WalkingDistance {

void load(const std::array<int, 16>& goal);
void generate(const std::array<int, 16>& goal);
void save();

int getIndex(const std::array<int, 16>& grid, bool alongRow = true);
int getHeuristic(int index);  // 2 bytes
int edge(int index, int dir, int tile);

};  // namespace WalkingDistance

#endif  // WALKINGDISTANCE_H