#include "../include/PartialDatabase.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>

#include "../include/Util.h"

#define INF 1000000

PartialDatabase::PartialDatabase(const std::array<int, 16>& grid,
                                 const std::string& dbName, int index)
    : filename("databases/def-" + std::to_string(index) + ".dat"),
      grid(grid),
      size(1) {
    // Calculate size: 16! / (16 - tiles.size())!
    std::cout << "Pattern #" << index << ":" << std::endl;
    for (int i = 0; i < 16; i++) {
        if (grid[i] > 0) {
            size *= 16 - tiles.size();
            tiles.push_back(grid[i]);
        }

        std::cout << std::setw(3) << grid[i];
        if (i % 4 == 3) {
            std::cout << std::endl;
        }
    }

    // Resize distMap
    distMap.resize(size, INF);

    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.good()) {
        // Database file missing, generate database
        std::cout << "Generating database" << std::endl;
        generateDists();
        saveDists();
    } else {
        // Read database from file
        std::cout << "Parsing database" << std::endl;

        uint64_t id = 0;
        unsigned dist;
        while (file.read((char*)&dist, sizeof(dist))) {
            distMap[id] = dist;
            id++;
        }

        if (id != size) {
            std::cout << "Error: incorrectly sized database!" << std::endl;
            throw;
        }
    }

    std::cout << "Number of entries: " << size << std::endl;
}

void PartialDatabase::generateDists() {
    PatternGroup group(grid);

    int count = 0;
    unsigned dist = 0;

    START_TIMER(pdb);

    std::queue<std::pair<Pattern, int>> bfs;
    const auto& start = group.initPattern;
    bfs.push({start, 0});
    distMap[start.id] = 0;

    while (!bfs.empty()) {
        const auto& [curr, currDist] = bfs.front();

        // Logging
        if (currDist > dist) {
            std::cout << dist << ": " << count << std::endl;
            dist = currDist;
            count = 1;
        } else {
            count++;
        }

        for (auto tile : tiles) {
            for (int j = 0; j < 4; j++) {
                auto dir = static_cast<Direction>(j);
                if (group.canShift(curr, tile, dir)) {
                    auto next = group.shiftCell(curr, tile, dir);

                    // Haven't found this board yet
                    if (distMap[next.id] == INF) {
                        distMap[next.id] = currDist + 1;
                        bfs.push({std::move(next), currDist + 1});
                    }
                }
            }
        }

        bfs.pop();
    }

    END_TIMER(pdb);
}

void PartialDatabase::saveDists() {
    // Store file
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.good()) {
        std::cerr << "Could not generate database file: " + filename
                  << std::endl;
        return;
    }

    for (auto dist : distMap) {
        file.write((char*)&dist, sizeof(dist));
    }
}
