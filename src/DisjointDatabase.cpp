#include "../include/DisjointDatabase.h"

#include <iostream>

DisjointDatabase::DisjointDatabase(
    const std::string& name,
    std::vector<std::vector<std::vector<uint>>> grids) {
    const auto WIDTH = grids[0][0].size();
    const auto HEIGHT = grids[0].size();

    where = std::vector<int>(WIDTH * HEIGHT, -1);

    // All partial grids, layered to one
    // This represents the solved grid
    std::vector<std::vector<int>> combined(HEIGHT, std::vector<int>(WIDTH, 0));

    // The reflected positions of the tiles
    mirrPos = std::vector<uint>(WIDTH * HEIGHT, 0);

    for (size_t i = 0; i < grids.size(); i++) {
        auto pd = std::make_unique<PartialDatabase>(grids[i], name, i);

        for (auto tile : pd->tiles) {
            if (where[tile] != -1) {
                std::cout << "Error: patterns overlapping" << std::endl;
                throw;
            }

            where[tile] = i;
        }

        databases.push_back(std::move(pd));

        // Calculate value to position mapping
        for (uint y = 0; y < HEIGHT; y++) {
            for (uint x = 0; x < WIDTH; x++) {
                if (grids[i][y][x] > 0) {
                    combined[y][x] = grids[i][y][x];
                }
            }
        }
    }

    for (uint y = 0; y < HEIGHT; y++) {
        for (uint x = 0; x < WIDTH; x++) {
            if (combined[y][x] > 0) {
                mirrPos[combined[y][x]] = combined[x][y];
            }
        }
    }

    for (auto i : mirrPos) std::cout << i << " ";
    std::cout << std::endl;

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
        }
    }
}

std::size_t DisjointDatabase::numPatterns() const { return databases.size(); }

int DisjointDatabase::getHeuristic(
    const std::vector<uint64_t>& patterns) const {
    int totalDist = 0;
    for (size_t i = 0; i < patterns.size(); i++) {
        totalDist += databases[i]->distMap[patterns[i]];
    }
    return totalDist;
}

DisjointDatabase::~DisjointDatabase() = default;
