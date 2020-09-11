#ifndef WALKINGDISTANCE_H
#define WALKINGDISTANCE_H

#include <array>
#include <string>
#include <vector>

constexpr auto tableSize = 24964;

class WalkingDistance {
    std::string filename;

    void generateDists();
    void saveDists();

public:
    WalkingDistance(const std::array<int, 16>& solved);

    int getHeuristic(const int index) const;

    std::array<uint64_t, tableSize> tables;
    std::array<int, tableSize> costs;
    std::array<std::array<std::array<int, 4>, 2>, tableSize> edges;
};

#endif  // WALKINGDISTANCE_H
