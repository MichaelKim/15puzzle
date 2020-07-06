#include "../include/DisjointDatabase.h"

#include <iostream>

DisjointDatabase::DisjointDatabase(
    const std::string& name, const std::vector<std::array<int, 16>>& grids) {
    const auto WIDTH = 4;
    const auto HEIGHT = 4;

    where.fill(-1);

    // All partial grids, layered to one
    // This represents the solved grid
    std::array<int, 16> combined;
    combined.fill(0);

    // The reflected positions of the tiles
    mirrPos.fill(0);

    for (auto i = 0; i < grids.size(); i++) {
        PartialDatabase pd(grids[i], name, i);

        for (auto tile : pd.tiles) {
            if (where[tile] != -1) {
                std::cout << "Error: patterns overlapping" << std::endl;
                throw;
            }

            where[tile] = i;
        }

        databases.push_back(pd);

        // Calculate value to position mapping
        for (int j = 0; j < 16; j++) {
            if (grids[i][j] > 0) {
                combined[j] = grids[i][j];
            }
        }
    }

    for (int j = 0; j < 16; j++) {
        if (combined[j] > 0) {
            int y = j / 4;
            int x = j % 4;
            int mirr = x * 4 + y;
            mirrPos[combined[mirr]] = combined[j];
        }
    }

    for (auto i : mirrPos) std::cout << i << " ";
    std::cout << std::endl;

    // Check that the patterns perfectly cover the board
    // (except for the blank tile)
    bool foundBlank = false;
    for (auto i : where) {
        if (i == -1) {
            if (foundBlank) {
                std::cout << "Error: found multiple blank tiles" << std::endl;
                throw;
            }

            // Assume blank tile
            foundBlank = true;
        }
    }
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
