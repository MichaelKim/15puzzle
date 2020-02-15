#include "../include/PartialDatabase.h"

#include "../include/Pattern.h"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <unordered_map>

PartialDatabase::PartialDatabase(std::vector<std::vector<int>> grid,
                                 std::string dbName, int index)
    : filename("databases/" + dbName + "-" + std::to_string(index) + ".dat"),
      grid(grid),
      WIDTH(grid[0].size()),
      HEIGHT(grid.size()),
      size(1) {
    // Get tiles
    std::cout << "Pattern #" << index << ":" << std::endl;
    for (uint y = 0; y < HEIGHT; y++) {
        for (uint x = 0; x < WIDTH; x++) {
            if (grid[y][x] > 0) {
                tiles.push_back(grid[y][x]);
            }

            std::cout << std::setw(3) << grid[y][x];
        }
        std::cout << std::endl;
    }

    // Calculate size: (WIDTH * HEIGHT)! / (WIDTH * HEIGHT - tiles.size())!
    for (size_t i = 0; i < tiles.size(); i++) {
        size *= WIDTH * HEIGHT - i;
    }

    // Resize distMap
    distMap.resize(size, 1000000);

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
        uint dist;
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
    std::cout << "Precomputing..." << std::endl;

    // Calculate deltas
    // delta[tiles[i]] = (WIDTH * HEIGHT - i)! /
    //                   (WIDTH * HEIGHT - tiles.size())!
    std::vector<int> deltas(WIDTH * HEIGHT, 1);
    for (int i = tiles.size() - 2; i >= 0; i--) {
        deltas[tiles[i]] = deltas[tiles[i + 1]] * (WIDTH * HEIGHT - 1 - i);
    }

    // Calculate compressed grid
    uint64_t startG = 0;
    for (int y = HEIGHT - 1; y >= 0; y--) {
        for (int x = WIDTH - 1; x >= 0; x--) {
            startG = (startG << 4) + grid[y][x];
        }
    }

    // Calculate starting ID and starting position
    std::vector<uint> startPos(WIDTH * HEIGHT, 0);
    std::unordered_map<int, int> before;

    for (uint y = 0; y < HEIGHT; y++) {
        for (uint x = 0; x < WIDTH; x++) {
            if (grid[y][x] > 0) {
                // New tile found
                uint beforeCount = 0;

                // Count number of preceding pattern tiles that's smaller
                for (auto& it : before) {
                    if (it.first < grid[y][x]) {
                        beforeCount++;
                    }
                }
                before[grid[y][x]] = beforeCount;
                startPos[grid[y][x]] = y * WIDTH + x;
            }
        }
    }

    uint64_t startId = 0;
    uint j = WIDTH * HEIGHT;
    for (auto tile : tiles) {
        startId *= j--;
        startId += startPos[tile] - before[tile];
    }

    // The pattern database consists of all possible permutations
    // of the pattern's tiles placed in the board. Any tile that
    // isn't part of the pattern are treated as blanks, so that
    // the board only contains the pattern's tiles.
    // To generate the database, it starts from the solved state
    // and runs BFS, sliding the pattern's tiles around
    // to generate all pattern states with heuristic 1,
    // then 2, etc. This ensures that if the search encounters
    // a new pattern state, it was reached with the minimum
    // number of slides.

    int count = 0;
    uint dist = 0;

    std::cout << "Generating database" << std::endl;

    auto startTime = std::chrono::steady_clock::now();

    // Start of BFS
    std::queue<Pattern> bfs;

    bfs.push({startId, 0, startPos, startG, WIDTH, HEIGHT});
    distMap[startId] = 0;

    while (!bfs.empty()) {
        auto& curr = bfs.front();

        // Logging
        if (curr.dist > dist) {
            std::cout << dist << ": " << count << std::endl;
            dist = curr.dist;
            count = 1;
        } else {
            count++;
        }

        for (auto tile : tiles) {
            for (uint j = 0; j < 4; j++) {
                Direction dir = static_cast<Direction>(j);
                if (curr.canShift(tile, dir)) {
                    auto next = curr.shiftCell(tile, dir, deltas);

                    // Haven't found this board yet
                    if (distMap[next.id] == 1000000) {
                        distMap[next.id] = next.dist;
                        bfs.push(next);
                    }
                }
            }
        }

        bfs.pop();
    }

    auto endTime = std::chrono::steady_clock::now();
    auto duration = (std::chrono::duration_cast<std::chrono::microseconds>(
                         endTime - startTime)
                         .count()) /
                    1000000.0;
    std::cout << "Time taken: " << duration << std::endl;
}

void PartialDatabase::saveDists() {
    // Store file
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.good()) {
        std::cerr << "Could not generate database file: " + filename
                  << std::endl;
    } else {
        for (auto dist : distMap) {
            if (dist == 1000000) {
                std::cout << "Error: missing entries!" << std::endl;
                throw;
            }
            file.write((char*)&dist, sizeof(dist));
        }

        file.close();
    }
}

PartialDatabase::~PartialDatabase() {}
