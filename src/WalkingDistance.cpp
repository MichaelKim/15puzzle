#include "../include/WalkingDistance.h"

#include <algorithm>
#include <fstream>
#include <limits>

#include "../include/Util.h"

using Board = std::vector<int>;
using Table = std::vector<std::vector<int>>;
using Hash = uint_fast64_t;            // 48-bit encoding
using Cost = WalkingDistance::Cost;    // Max = 36
using Index = WalkingDistance::Index;  // Max = TABLE_SIZE

using WalkingDistance::col;
using WalkingDistance::costs;
using WalkingDistance::edges;
using WalkingDistance::height;
using WalkingDistance::row;
using WalkingDistance::width;

std::vector<Hash> tables;
std::vector<Cost> WalkingDistance::costs;
std::vector<std::array<std::vector<Index>, 2>> WalkingDistance::edges;

std::vector<int> WalkingDistance::row;  // Row #
std::vector<int> WalkingDistance::col;  // Column #

int WalkingDistance::width;
int WalkingDistance::height;

Table calculateTable(const Board& grid, bool alongRow = true) {
    Table table(height, std::vector<int>(width, 0));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int tile = grid[y * width + x];
            if (tile > 0) {
                if (alongRow) {
                    table[y][row[tile]]++;
                } else {
                    table[x][col[tile]]++;
                }
            }
        }
    }

    return table;
}

Hash calculateHash(const Table& table) {
    // Compress WD tables
    Hash hash = 0;

    for (auto& row : table) {
        for (auto val : row) {
            hash = (hash << 3) | val;
        }
    }

    return hash;
}

std::pair<Table, int> hashToTable(Hash hash) {
    Table table(height, std::vector<int>(width, 0));
    int rowSpace = 0;

    for (int y = height - 1; y >= 0; y--) {
        int count = 0;
        for (int x = width - 1; x >= 0; x--) {
            table[y][x] = hash & 7;
            hash >>= 3;

            count += table[y][x];
        }
        if (count == width - 1) {
            rowSpace = y;
        }
    }

    return {table, rowSpace};
}

void generate(const Board& goal) {
    // Initial table
    auto comp = calculateHash(calculateTable(goal));

    // Helper for assigning new edges quickly
    std::array<std::vector<Index>, 2> newEdges;
    newEdges.fill(std::vector<Index>(width, std::numeric_limits<Index>::max()));

    auto add = [&newEdges](const Table& table, Cost cost) {
        auto newComp = calculateHash(table);

        auto it = std::find(tables.begin(), tables.end(), newComp);
        auto index = std::distance(tables.begin(), it);
        if (it == tables.end()) {
            tables.push_back(newComp);
            costs.push_back(cost);
            edges.push_back(newEdges);
        }
        return index;
    };

    // Start of BFS
    assertm(tables.size() == 0,
            "Tables should be empty at start of generation");
    assertm(costs.size() == 0, "Tables should be empty at start of generation");
    assertm(edges.size() == 0, "Tables should be empty at start of generation");
    tables.push_back(comp);
    costs.push_back(0);
    edges.push_back(newEdges);

    for (int left = 0; left < tables.size(); left++) {
        auto currTable = tables[left];
        Cost currCost = costs[left] + 1;

        auto [table, rowSpace] = hashToTable(currTable);

        if (int rowTile = rowSpace + 1; rowTile < height) {
            for (int x = 0; x < width; x++) {
                if (table[rowTile][x]) {
                    table[rowTile][x]--;
                    table[rowSpace][x]++;

                    auto index = add(table, currCost);

                    edges[left][0][x] = index;
                    edges[index][1][x] = left;

                    table[rowTile][x]++;
                    table[rowSpace][x]--;
                }
            }
        }

        if (int rowTile = rowSpace - 1; rowTile >= 0) {
            for (int x = 0; x < width; x++) {
                if (table[rowTile][x]) {
                    table[rowTile][x]--;
                    table[rowSpace][x]++;

                    auto index = add(table, currCost);

                    edges[left][1][x] = index;
                    edges[index][0][x] = left;

                    table[rowTile][x]++;
                    table[rowSpace][x]--;
                }
            }
        }
    }
}

void save(std::string filename) {
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.good()) {
        std::cerr << "Could not generate database file: databases/def-wd.dat"
                  << std::endl;
        return;
    }

    int size = tables.size();
    file.write(reinterpret_cast<char*>(&size), sizeof(size));

    for (int i = 0; i < size; i++) {
        file.write(reinterpret_cast<char*>(&tables[i]), sizeof(tables[i]));
        file.write(reinterpret_cast<char*>(&costs[i]), sizeof(costs[i]));
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < width; k++) {
                file.write(reinterpret_cast<char*>(&edges[i][j][k]),
                           sizeof(edges[i][j][k]));
            }
        }
    }
}

void WalkingDistance::load(const std::vector<int>& goal, int w, int h) {
    assertm(w == h, "Walking Distance requires square boards");

    width = w;
    height = h;
    auto length = w * h;

    row.resize(length);
    col.resize(length);

    // Calculate row / column indices
    for (int i = 0; i < length; i++) {
        row[goal[i]] = i / width;
        col[goal[i]] = i % width;
    }

    std::string filename = "databases/" + std::to_string(w) + "-wd.dat";
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.good()) {
        // Database file missing, generate database
        DEBUG("Generating WD database");
        generate(goal);
        save(filename);
        return;
    }

    // Read database from file
    DEBUG("Parsing database");

    // Store size in .dat file
    int size = 0;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));

    costs.resize(size);
    edges.resize(size);

    for (int i = 0; i < size; i++) {
        file.read(reinterpret_cast<char*>(&tables[i]), sizeof(tables[i]));
        file.read(reinterpret_cast<char*>(&costs[i]), sizeof(costs[i]));
        for (int j = 0; j < 2; j++) {
            edges[i][j].resize(width);
            for (int k = 0; k < width; k++) {
                file.read(reinterpret_cast<char*>(&edges[i][j][k]),
                          sizeof(edges[i][j][k]));
            }
        }
    }
}

int WalkingDistance::getIndex(const Board& grid, bool alongRow) {
    auto hash = calculateHash(calculateTable(grid, alongRow));

    // Convert to index
    auto it = std::find(tables.begin(), tables.end(), hash);
    assertm(it != tables.end(), "Missing walking distance table");
    auto index = std::distance(tables.begin(), it);

    return index;
}
