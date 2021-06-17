#include "../include/DisjointDatabase.h"

#include <execution>
#include <fstream>
#include <limits>
#include <numeric>
#include <queue>
#include <unordered_map>

#include "../include/Pattern.h"
#include "../include/Util.h"

using Grid = DisjointDatabase::Grid;
using Hash = DisjointDatabase::Hash;
using Cost = uint_fast8_t;

using DisjointDatabase::height;
using DisjointDatabase::tileDeltas;
using DisjointDatabase::where;
using DisjointDatabase::width;

constexpr Cost INF = std::numeric_limits<Cost>::max();  // 255

int DisjointDatabase::width;
int DisjointDatabase::height;

std::vector<std::vector<int>> patternTiles;
std::vector<std::vector<Cost>> costs;
Grid DisjointDatabase::where;
Grid DisjointDatabase::tileDeltas;
Grid DisjointDatabase::mirrPos;
Grid DisjointDatabase::mirror;

std::vector<Cost> generatePattern(const Grid& pattern, int size) {
    PatternGroup group(pattern, width, height);

    // For logging
    int count = 0;  // Per depth
    int total = 0;  // Entire search
    Cost dist = 0;  // Current depth

    std::queue<std::pair<Pattern, Cost>> bfs;
    const auto& start = group.initPattern;
    bfs.push({start, 0});

    std::vector<Cost> costs(size, INF);
    costs[start.id] = 0;

    while (!bfs.empty()) {
        const auto& [curr, currDist] = bfs.front();

        // Logging
        if (currDist > dist) {
            DEBUG((int)dist << ": " << count << ", "
                            << (int)(total * (100.f / size)) << "% (" << total
                            << "/" << size << ")");
            dist = currDist;
            count = 1;
        } else {
            count++;
        }
        total++;

        for (auto tile : group.tiles) {
            for (int j = 0; j < 4; j++) {
                auto dir = static_cast<Direction>(j);
                if (group.canShift(curr, tile, dir)) {
                    auto next = group.shiftCell(curr, tile, dir);

                    // Haven't found this board yet
                    if (costs[next.id] == INF) {
                        costs[next.id] = currDist + 1;
                        bfs.push({next, currDist + 1});
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

    for (int i = 1; i < width * height; i++) {  // Ignore blank tile (0)
        patternTiles[where[i]].push_back(i);
    }
}

void calculateDeltas() {
    tileDeltas = std::vector<int>(width * height, 1);

    for (auto& tiles : patternTiles) {
        for (int j = (int)tiles.size() - 2; j >= 0; j--) {
            tileDeltas[tiles[j]] =
                tileDeltas[tiles[j + 1]] * (width * height - 1 - j);
        }
    }
}

void DisjointDatabase::load(const std::vector<Grid>& patterns,
                            const std::string& name, int w, int h) {
    width = w;
    height = h;
    auto length = w * h;

    costs.clear();
    patternTiles.clear();
    where.resize(length, -1);
    mirrPos.resize(length, 0);
    mirror.resize(length);
    // TODO: test with blank not in top-left or bottom-right
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            mirror[y * width + x] = x * width + y;
        }
    }

    for (std::size_t i = 0; i < patterns.size(); i++) {
        DEBUG("Pattern #" << i << ':');

        int size = 1;      // # of entries in database
        int numTiles = 0;  // # of tiles in partial pattern

        for (int j = 0; j < length; j++) {
            int tile = patterns[i][j];
            if (tile > 0) {
                size *= length - numTiles;
                numTiles++;

                where[tile] = i;
            }
        }

        costs.push_back(loadPattern(
            patterns[i], "databases/" + name + "-" + std::to_string(i) + ".dat",
            size));
    }

    // All partial grids, layered to one
    // This represents the solved grid
    auto combined = combine(patterns);
    for (int i = 0; i < length; i++) {
        if (combined[i] > 0) {
            mirrPos[combined[i]] = combined[mirror[i]];
        }
    }

    calculatePatternTiles();
    calculateDeltas();
}

std::vector<Hash> DisjointDatabase::calculatePatterns(const Grid& grid) {
    auto length = width * height;
    std::vector<Hash> pat(costs.size(), 0);

    for (int i = 0; i < (int)costs.size(); i++) {
        // Calculate pattern
        std::vector<int> startPos(length, 0);
        std::unordered_map<int, int> before;
        for (int j = 0; j < length; j++) {
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

        int j = length;
        for (auto tile : patternTiles[i]) {
            pat[i] *= j--;
            pat[i] += startPos[tile] - before[tile];
        }
    }

    return pat;
}

int DisjointDatabase::getHeuristic(const std::vector<Hash>& patterns) {
    return std::transform_reduce(
        std::execution::par_unseq, costs.cbegin(), costs.cend(),
        patterns.cbegin(), 0, std::plus<>(),
        [](const auto& cost, const auto& pattern) { return cost[pattern]; });
}
