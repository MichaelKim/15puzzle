#include "../include/DisjointDatabase.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <unordered_map>

#include "../include/Util.h"

using Grid = DisjointDatabase::Grid;

std::vector<std::vector<int>> patternTiles;
std::vector<std::vector<int>> costs;
std::array<int, 16> DisjointDatabase::where;
Grid DisjointDatabase::tileDeltas;
Grid DisjointDatabase::mirrPos{};

std::vector<int> readDatabase(int index, int size) {
    std::ifstream file("databases/def-" + std::to_string(index) + ".dat",
                       std::ios::in | std::ios::binary);
    std::vector<int> cost(size);

    // Read database from file
    for (auto& c : cost) {
        file.read(reinterpret_cast<char*>(&c), sizeof(c));
    }

    return cost;
}

void calculatePatternTiles() {
    using DisjointDatabase::where;

    patternTiles.resize(costs.size());

    for (int i = 1; i < 16; i++) {  // Ignore blank tile (0)
        patternTiles[where[i]].push_back(i);
    }
}

void calculateDeltas() {
    using DisjointDatabase::tileDeltas;

    DisjointDatabase::tileDeltas.fill(1);

    for (auto& tiles : patternTiles) {
        for (int j = tiles.size() - 2; j >= 0; j--) {
            tileDeltas[tiles[j]] = tileDeltas[tiles[j + 1]] * (16 - 1 - j);
        }
    }
}

void DisjointDatabase::load(const std::vector<Grid>& grids) {
    // All partial grids, layered to one
    // This represents the solved grid
    Grid combined{};

    for (int i = 0; i < grids.size(); i++) {
        DEBUG("Pattern #" << i << ':');

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

        costs.push_back(readDatabase(i, size));
    }

    for (int i = 0; i < 16; i++) {
        if (combined[i] > 0) {
            mirrPos[combined[i]] = combined[mirror[i]];
        }
    }

    calculatePatternTiles();
    calculateDeltas();
}

std::vector<uint64_t> DisjointDatabase::calculatePatterns(const Grid& grid) {
    std::vector<uint64_t> pat(costs.size(), 0);

    for (int i = 0; i < costs.size(); i++) {
        const auto& tiles = patternTiles[i];

        // Calculate pattern
        std::vector<int> startPos(16, 0);
        std::unordered_map<int, int> before;
        for (int j = 0; j < 16; j++) {
            if (where[grid[j]] == i) {
                // New tile found
                int beforeCount = 0;

                // Count number of preceding pattern tiles that's smaller
                for (auto& it : before) {
                    if (it.first < grid[j]) {
                        beforeCount++;
                    }
                }

                before[grid[j]] = beforeCount;
                startPos[grid[j]] = j;
            }
        }

        int j = 16;
        for (auto tile : tiles) {
            pat[i] *= j--;
            pat[i] += startPos[tile] - before[tile];
        }
    }

    return pat;
}

int DisjointDatabase::getHeuristic(const std::vector<uint64_t>& patterns) {
    int totalDist = 0;
    for (size_t i = 0; i < patterns.size(); i++) {
        totalDist += costs[i][patterns[i]];
    }
    return totalDist;
}
