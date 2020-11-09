#include "../include/WalkingDistance.h"

#include <algorithm>
#include <execution>
#include <fstream>
#include <limits>
#include <numeric>

#include "../include/Util.h"

using Board = std::vector<int>;
using Table = std::vector<std::vector<int>>;
using Cost = WalkingDistance::Cost;
using Index = WalkingDistance::Index;

using WalkingDistance::col;
using WalkingDistance::costs;
using WalkingDistance::edgesDown;
using WalkingDistance::edgesUp;
using WalkingDistance::height;
using WalkingDistance::row;
using WalkingDistance::width;

std::vector<Table> tables;
std::vector<Cost> WalkingDistance::costs;
std::vector<std::vector<Index>> WalkingDistance::edgesUp;
std::vector<std::vector<Index>> WalkingDistance::edgesDown;

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

int getRowSpace(const Table& table) {
    for (int y = 0; y < height; y++) {
        auto count = std::reduce(std::execution::par_unseq, table[y].begin(),
                                 table[y].end());
        if (count == width - 1) {
            return y;
        }
    }
    assertm(false, "WD table must have a row with a blank");
}

int add(std::size_t left, int cost) {
    std::size_t index = 0;
    while (index < tables.size() &&
           (index == left || tables[index] != tables[left]))
        index++;
    if (index == tables.size()) {
        tables.push_back(tables[left]);
        costs.push_back(cost);
        edgesUp.emplace_back(width, std::numeric_limits<Index>::max());
        edgesDown.emplace_back(width, std::numeric_limits<Index>::max());
    }
    return index;
}

void generate(const Board& goal) {
    // Start of BFS
    assertm(tables.size() == 0,
            "Tables should be empty at start of generation");
    assertm(costs.size() == 0, "Tables should be empty at start of generation");
    assertm(edgesUp.size() == 0,
            "Tables should be empty at start of generation");
    assertm(edgesDown.size() == 0,
            "Tables should be empty at start of generation");

    // Initial table (goal)
    tables.push_back(calculateTable(goal));
    costs.push_back(0);
    edgesUp.emplace_back(width, std::numeric_limits<Index>::max());
    edgesDown.emplace_back(width, std::numeric_limits<Index>::max());

    for (std::size_t left = 0; left < tables.size(); left++) {
        auto cost = costs[left] + 1;
        auto rowSpace = getRowSpace(tables[left]);

        if (int rowTile = rowSpace + 1; rowTile < height) {
            for (int x = 0; x < width; x++) {
                if (tables[left][rowTile][x]) {
                    tables[left][rowTile][x]--;
                    tables[left][rowSpace][x]++;

                    auto index = add(left, cost);

                    edgesUp[left][x] = index;
                    edgesDown[index][x] = left;

                    tables[left][rowTile][x]++;
                    tables[left][rowSpace][x]--;
                }
            }
        }

        if (int rowTile = rowSpace - 1; rowTile >= 0) {
            for (int x = 0; x < width; x++) {
                if (tables[left][rowTile][x]) {
                    tables[left][rowTile][x]--;
                    tables[left][rowSpace][x]++;

                    auto index = add(left, cost);

                    edgesDown[left][x] = index;
                    edgesUp[index][x] = left;

                    tables[left][rowTile][x]++;
                    tables[left][rowSpace][x]--;
                }
            }
        }
    }
}

void save(const std::string& filename) {
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.good()) {
        std::cerr << "Could not generate database file: databases/def-wd.dat"
                  << std::endl;
        return;
    }

    int size = tables.size();
    assertm((int)costs.size() == size, "Mismatching table sizes");
    assertm((int)edgesUp.size() == size, "Mismatching table sizes");
    assertm((int)edgesDown.size() == size, "Mismatching table sizes");
    file.write(reinterpret_cast<char*>(&size), sizeof(size));

    for (auto& table : tables) {
        for (auto& row : table) {
            for (auto tile : row) {
                file.write(reinterpret_cast<char*>(&tile), sizeof(tile));
            }
        }
    }
    for (auto cost : costs) {
        file.write(reinterpret_cast<char*>(&cost), sizeof(cost));
    }
    for (auto& edge : edgesUp) {
        for (auto col : edge) {
            file.write(reinterpret_cast<char*>(&col), sizeof(col));
        }
    }
    for (auto& edge : edgesDown) {
        for (auto col : edge) {
            file.write(reinterpret_cast<char*>(&col), sizeof(col));
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

    tables.resize(size);
    costs.resize(size);
    edgesUp.resize(size);
    edgesDown.resize(size);

    for (auto& table : tables) {
        table.resize(height);
        for (auto& row : table) {
            row.resize(width);
            for (auto& tile : row) {
                file.read(reinterpret_cast<char*>(&tile), sizeof(tile));
            }
        }
    }
    for (auto& cost : costs) {
        file.read(reinterpret_cast<char*>(&cost), sizeof(cost));
    }
    for (auto& edge : edgesUp) {
        edge.resize(width);
        for (auto& col : edge) {
            file.read(reinterpret_cast<char*>(&col), sizeof(col));
        }
    }
    for (auto& edge : edgesDown) {
        edge.resize(width);
        for (auto& col : edge) {
            file.read(reinterpret_cast<char*>(&col), sizeof(col));
        }
    }
}

int WalkingDistance::getIndex(const Board& grid, bool alongRow) {
    auto table = calculateTable(grid, alongRow);

    // Convert to index
    auto it = std::find(tables.cbegin(), tables.cend(), table);
    assertm(it != tables.end(), "Missing walking distance table");
    auto index = std::distance(tables.cbegin(), it);

    return index;
}
