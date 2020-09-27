#include "../include/WalkingDistance.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <unordered_map>

#include "../include/Util.h"

using Board = std::array<int, 16>;
using Table = std::array<std::array<int, 4>, 4>;
using Hash = uint_fast64_t;

using WalkingDistance::costs;
using WalkingDistance::edges;
using WalkingDistance::TABLE_SIZE;

std::array<uint_fast64_t, TABLE_SIZE> tables;
std::array<int, TABLE_SIZE> WalkingDistance::costs;
std::array<std::array<std::array<int, 4>, 2>, TABLE_SIZE>
    WalkingDistance::edges;

std::array<std::array<int, 4>, 4> calculateTable(
    const std::array<int, 16>& grid, bool alongRow = true) {
    Table table{};

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            int tile = grid[y * 4 + x];
            if (tile > 0) {
                if (alongRow) {
                    table[y][(tile - 1) / 4]++;
                } else {
                    table[x][(tile - 1) % 4]++;
                }
            }
        }
    }

    return table;
}

Hash calculateHash(const Table& table) {
    // Compress WD tables
    Hash hash = 0;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            hash = (hash << 3) | table[y][x];
        }
    }

    return hash;
}

std::pair<Table, int> hashToTable(Hash hash) {
    Table table{};
    int rowSpace = 0;

    for (int y = 3; y >= 0; y--) {
        int count = 0;
        for (int x = 3; x >= 0; x--) {
            table[y][x] = hash & 7;
            hash >>= 3;

            count += table[y][x];
        }
        if (count == 3) {
            rowSpace = y;
        }
    }

    return {table, rowSpace};
}

void generate(const std::array<int, 16>& goal) {
    // Initial table
    auto comp = calculateHash(calculateTable(goal));

    // Start of BFS
    tables[0] = comp;
    costs[0] = 0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 4; j++) {
            edges[0][i][j] = TABLE_SIZE;
        }
    }

    for (int left = 0, right = 1; left < right; left++) {
        auto currTable = tables[left];
        int currCost = costs[left] + 1;

        auto [table, rowSpace] = hashToTable(currTable);

        if (int rowTile = rowSpace + 1; rowTile < 4) {
            for (int x = 0; x < 4; x++) {
                if (table[rowTile][x]) {
                    table[rowTile][x]--;
                    table[rowSpace][x]++;

                    auto newComp = calculateHash(table);

                    int i = 0;
                    while (i < right && tables[i] != newComp) i++;
                    if (i == right) {
                        tables[right] = newComp;
                        costs[right] = currCost;
                        for (int j = 0; j < 2; j++) {
                            for (int k = 0; k < 4; k++) {
                                edges[right][j][k] = TABLE_SIZE;
                            }
                        }
                        right++;
                    }

                    edges[left][0][x] = i;
                    edges[i][1][x] = left;

                    table[rowTile][x]++;
                    table[rowSpace][x]--;
                }
            }
        }

        if (int rowTile = rowSpace - 1; rowTile >= 0) {
            for (int x = 0; x < 4; x++) {
                if (table[rowTile][x]) {
                    table[rowTile][x]--;
                    table[rowSpace][x]++;

                    auto newComp = calculateHash(table);

                    int i = 0;
                    while (i < right && tables[i] != newComp) i++;
                    if (i == right) {
                        tables[right] = newComp;
                        costs[right] = currCost;
                        for (int j = 0; j < 2; j++) {
                            for (int k = 0; k < 4; k++) {
                                edges[right][j][k] = TABLE_SIZE;
                            }
                        }
                        right++;
                    }

                    edges[left][1][x] = i;
                    edges[i][0][x] = left;

                    table[rowTile][x]++;
                    table[rowSpace][x]--;
                }
            }
        }
    }
}

void save() {
    std::ofstream file("databases/def-wd.dat",
                       std::ios::out | std::ios::binary);
    if (!file.good()) {
        std::cerr << "Could not generate database file: databases/def-wd.dat"
                  << std::endl;
        return;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        file.write(reinterpret_cast<char*>(&tables[i]), sizeof(tables[i]));
        file.write(reinterpret_cast<char*>(&costs[i]), sizeof(costs[i]));
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 4; k++) {
                file.write(reinterpret_cast<char*>(&edges[i][j][k]),
                           sizeof(edges[i][j][k]));
            }
        }
    }
}

void WalkingDistance::load(const Board& goal) {
    std::ifstream file("databases/def-wd.dat", std::ios::in | std::ios::binary);
    if (!file.good()) {
        // Database file missing, generate database
        DEBUG("Generating WD database");
        generate(goal);
        save();
        return;
    }

    // Read database from file
    DEBUG("Parsing database");

    for (int i = 0; i < TABLE_SIZE; i++) {
        file.read(reinterpret_cast<char*>(&tables[i]), sizeof(tables[i]));
        file.read(reinterpret_cast<char*>(&costs[i]), sizeof(costs[i]));
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 4; k++) {
                file.read(reinterpret_cast<char*>(&edges[i][j][k]),
                          sizeof(edges[i][j][k]));
            }
        }
    }
}

int WalkingDistance::getIndex(const std::array<int, 16>& grid, bool alongRow) {
    auto hash = calculateHash(calculateTable(grid, alongRow));

    // Convert to index
    auto it = std::find(tables.begin(), tables.end(), hash);
    assertm(it != tables.end(), "Missing walking distance table");
    auto rowIndex = std::distance(tables.begin(), it);

    return rowIndex;
}
