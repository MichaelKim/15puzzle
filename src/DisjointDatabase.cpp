#include "../include/DisjointDatabase.h"

#include <fstream>
#include <queue>
#include <unordered_map>

#include "../include/Pattern.h"
#include "../include/Util.h"

using Grid = DisjointDatabase::Grid;
using Hash = DisjointDatabase::Hash;
using Cost = uint_fast8_t;

using DisjointDatabase::tileDeltas;
using DisjointDatabase::where;

constexpr Cost INF = 81;

std::vector<std::vector<int>> patternTiles;
std::vector<std::vector<Cost>> costs;
Grid DisjointDatabase::where;
Grid DisjointDatabase::tileDeltas;
Grid DisjointDatabase::mirrPos{};

std::vector<Cost> generatePattern(const Grid& pattern, int size) {
    PatternGroup group(pattern);

    // For logging
    int count = 0;
    Cost dist = 0;

    std::queue<std::pair<Pattern, Cost>> bfs;
    const auto& start = group.initPattern;
    bfs.push({start, 0});

    std::vector<Cost> costs(size, INF);
    costs[start.id] = 0;

    while (!bfs.empty()) {
        const auto& [curr, currDist] = bfs.front();

        // Logging
        if (currDist > dist) {
            DEBUG((int)dist << ": " << count);
            dist = currDist;
            count = 1;
        } else {
            count++;
        }

        for (auto tile : group.tiles) {
            for (int j = 0; j < 4; j++) {
                auto dir = static_cast<Direction>(j);
                if (group.canShift(curr, tile, dir)) {
                    auto next = group.shiftCell(curr, tile, dir);

                    // Haven't found this board yet
                    if (costs[next.id] == INF) {
                        costs[next.id] = currDist + 1;
                        bfs.push({std::move(next), currDist + 1});
                    }
                }
            }
        }

        bfs.pop();
    }

    return costs;
}

void savePattern(const std::vector<Cost>& costs, const std::string& filename) {
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.good()) {
        std::cerr << "Could not generate database file: " + filename
                  << std::endl;
        return;
    }

    for (auto c : costs) {
        file.write(reinterpret_cast<char*>(&c), sizeof(c));
    }
}

std::vector<Cost> loadPattern(const Grid& pattern, const std::string& filename,
                              int size) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.good()) {
        // Database file missing, generate database
        DEBUG("Generating pattern");

        auto cost = generatePattern(pattern, size);
        savePattern(cost, filename);
        return cost;
    }

    // Read database from file
    DEBUG("Parsing pattern");

    std::vector<Cost> cost(size);

    // Read database from file
    for (auto& c : cost) {
        file.read(reinterpret_cast<char*>(&c), sizeof(c));
    }

    return cost;
}

void calculatePatternTiles() {
    patternTiles.resize(costs.size());

    for (int i = 1; i < 16; i++) {  // Ignore blank tile (0)
        patternTiles[where[i]].push_back(i);
    }
}

void calculateDeltas() {
    DisjointDatabase::tileDeltas.fill(1);

    for (auto& tiles : patternTiles) {
        for (int j = tiles.size() - 2; j >= 0; j--) {
            tileDeltas[tiles[j]] = tileDeltas[tiles[j + 1]] * (16 - 1 - j);
        }
    }
}

void DisjointDatabase::load(const std::vector<Grid>& patterns) {
    // All partial grids, layered to one
    // This represents the solved grid
    Grid combined{};

    for (int i = 0; i < patterns.size(); i++) {
        DEBUG("Pattern #" << i << ':');

        int size = 1;      // # of entries in database
        int numTiles = 0;  // # of tiles in partial pattern

        for (int j = 0; j < 16; j++) {
            int tile = patterns[i][j];
            if (tile > 0) {
                size *= 16 - numTiles;
                numTiles++;

                where[tile] = i;
                combined[j] = tile;
            } else {
                where[tile] = -1;
            }
        }

        costs.push_back(loadPattern(
            patterns[i], "databases/def-" + std::to_string(i) + ".dat", size));
    }

    for (int i = 0; i < 16; i++) {
        if (combined[i] > 0) {
            mirrPos[combined[i]] = combined[mirror[i]];
        }
    }

    calculatePatternTiles();
    calculateDeltas();
}

std::vector<Hash> DisjointDatabase::calculatePatterns(const Grid& grid) {
    std::vector<Hash> pat(costs.size(), 0);

    for (int i = 0; i < costs.size(); i++) {
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
        for (auto tile : patternTiles[i]) {
            pat[i] *= j--;
            pat[i] += startPos[tile] - before[tile];
        }
    }

    return pat;
}

int DisjointDatabase::getHeuristic(const std::vector<Hash>& patterns) {
    int totalDist = 0;
    for (size_t i = 0; i < patterns.size(); i++) {
        totalDist += costs[i][patterns[i]];
    }
    return totalDist;
}
