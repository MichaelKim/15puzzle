#include "../include/DisjointDatabase.h"

#include <iostream>

DisjointDatabase::DisjointDatabase(
    int len, std::string name, std::vector<std::vector<std::vector<int>>> grids)
    : where(len, -1) {
    for (size_t i = 0; i < grids.size(); i++) {
        auto pd = std::make_shared<PartialDatabase>(grids[i], name, i);
        databases.push_back(pd);

        for (auto tile : pd->tiles) {
            if (where[tile] != -1) {
                std::cout << "Error: patterns overlapping" << std::endl;
                throw;
            }

            where[tile] = i;
        }
    }

    // Check that the patterns perfectly cover the board
    // (except for the blank tile)
    bool foundBlank = false;
    for (auto& i : where) {
        if (i == -1) {
            if (foundBlank) {
                std::cout << "Error: found multiple blank tiles" << std::endl;
                throw;
            }

            // Assume blank tile
            foundBlank = true;
            i = 0;
        }
    }
}

int DisjointDatabase::numPatterns() const { return databases.size(); }

int DisjointDatabase::getHeuristic(
    const std::vector<uint64_t>& positions) const {
    int totalDist = 0;
    for (size_t i = 0; i < positions.size(); i++) {
        totalDist += databases[i]->distMap[positions[i]];
    }
    return totalDist;
}

DisjointDatabase::~DisjointDatabase() {}
