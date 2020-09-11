#include "../include/WalkingDistance.h"

#include <fstream>
#include <iostream>

WalkingDistance::WalkingDistance(const std::array<int, 16>& solved)
    : filename("databases/def-wd.dat") {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.good()) {
        // Database file missing, generate database
        std::cout << "Generating WD database" << std::endl;
        generateDists();
        saveDists();
    } else {
        // Read database from file
        std::cout << "Parsing database" << std::endl;

        uint64_t table;
        int cost;
        int edge;
        for (int i = 0; i < tableSize; i++) {
            file.read((char*)&table, sizeof(table));
            tables[i] = table;

            file.read((char*)&cost, sizeof(cost));
            costs[i] = cost;

            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 4; k++) {
                    file.read((char*)&edge, sizeof(edge));
                    edges[i][j][k] = edge;
                }
            }
        }
    }
}

void WalkingDistance::generateDists() {
    // Initial table
    std::array<std::array<int, 4>, 4> initTable = {
        {{4, 0, 0, 0}, {0, 4, 0, 0}, {0, 0, 4, 0}, {0, 0, 0, 3}}};

    uint64_t comp = 0;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            comp = (comp << 3) | initTable[y][x];
        }
    }

    // Start of BFS
    tables[0] = comp;
    costs[0] = 0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 4; j++) {
            edges[0][i][j] = tableSize;
        }
    }

    for (int left = 0, right = 1; left < right; left++) {
        uint64_t currTable = tables[left];
        int currCost = costs[left] + 1;

        std::array<std::array<int, 4>, 4> table;
        int rowSpace = 0;
        for (int y = 3; y >= 0; y--) {
            int count = 0;
            for (int x = 3; x >= 0; x--) {
                table[y][x] = currTable & 7;
                currTable >>= 3;

                count += table[y][x];
            }
            if (count == 3) {
                rowSpace = y;
            }
        }

        if (int rowTile = rowSpace + 1; rowTile < 4) {
            for (int x = 0; x < 4; x++) {
                if (table[rowTile][x]) {
                    table[rowTile][x]--;
                    table[rowSpace][x]++;

                    uint64_t newComp = 0;
                    for (int y = 0; y < 4; y++) {
                        for (int x = 0; x < 4; x++) {
                            newComp = (newComp << 3) | table[y][x];
                        }
                    }

                    int i = 0;
                    while (i < right && tables[i] != newComp) i++;
                    if (i == right) {
                        tables[right] = newComp;
                        costs[right] = currCost;
                        for (int j = 0; j < 2; j++) {
                            for (int k = 0; k < 4; k++) {
                                edges[right][j][k] = tableSize;
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

                    uint64_t newComp = 0;
                    for (int y = 0; y < 4; y++) {
                        for (int x = 0; x < 4; x++) {
                            newComp = (newComp << 3) | table[y][x];
                        }
                    }

                    int i = 0;
                    while (i < right && tables[i] != newComp) i++;
                    if (i == right) {
                        tables[right] = newComp;
                        costs[right] = currCost;
                        for (int j = 0; j < 2; j++) {
                            for (int k = 0; k < 4; k++) {
                                edges[right][j][k] = tableSize;
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

void WalkingDistance::saveDists() {
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.good()) {
        std::cerr << "Could not generate database file: " + filename
                  << std::endl;
        return;
    }

    for (int i = 0; i < tableSize; i++) {
        file.write((char*)&tables[i], sizeof(tables[i]));
        file.write((char*)&costs[i], sizeof(costs[i]));
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 4; k++) {
                file.write((char*)&edges[i][j][k], sizeof(edges[i][j][k]));
            }
        }
    }
}

int WalkingDistance::getHeuristic(const int index) const {
    return costs[index];
}
