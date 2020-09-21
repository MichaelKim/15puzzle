#include "../include/DisjointDatabase.h"

#include <fstream>
#include <iomanip>
#include <iostream>

DisjointDatabase::DisjointDatabase(
    const std::string& name, const std::vector<std::array<int, 16>>& grids)
    : mirrPos({}) {
    // All partial grids, layered to one
    // This represents the solved grid
    std::array<int, 16> combined{};
    std::vector<std::vector<int>> costs;

    for (int i = 0; i < grids.size(); i++) {
        std::cout << "Pattern #" << i << ":" << std::endl;
        int size = 1;      // # of entries in database
        int numTiles = 0;  // # of tiles in partial pattern

        for (int j = 0; j < 16; j++) {
            int tile = grids[i][j];
            if (tile > 0) {
                size *= 16 - numTiles;
                numTiles++;

                where[tile] = i;
                combined[j] = tile;
            } else {
                where[tile] = -1;
            }
        }

        costs.push_back(
            loadDatabase("databases/def-" + std::to_string(i) + ".dat", size));
    }

    for (int i = 0; i < 16; i++) {
        if (combined[i] > 0) {
            int y = i / 4;
            int x = i % 4;
            int mirr = x * 4 + y;
            mirrPos[combined[mirr]] = combined[i];
        }
    }
}

std::vector<int> DisjointDatabase::loadDatabase(std::string filename,
                                                int size) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    std::vector<int> cost(size);
    // Read database from file
    uint64_t id = 0;
    unsigned dist;
    while (file.read((char*)&dist, sizeof(dist))) {
        cost[id] = dist;
        id++;
    }
    return cost;
}

std::size_t DisjointDatabase::numPatterns() const { return databases.size(); }

int DisjointDatabase::getHeuristic(
    const std::vector<uint64_t>& patterns) const {
    int totalDist = 0;
    for (size_t i = 0; i < patterns.size(); i++) {
        totalDist += databases[i].distMap[patterns[i]];
    }
    return totalDist;
}
