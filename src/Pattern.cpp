#include "../include/Pattern.h"

#include <iomanip>
#include <iostream>
#include <map>
#include <ostream>
#include <unordered_map>
#include <vector>

Pattern::Pattern(std::vector<std::vector<int>> g)
    : grid(0), WIDTH(g[0].size()), HEIGHT(g.size()), id(0) {
    // Sort by tile value
    std::map<int, int> patterns;
    std::unordered_map<int, Point> invp;

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (g[y][x] > 0) {
                // New tile found
                int beforeCount = 0;

                // Count number of preceding pattern tiles that's smaller
                for (auto it : patterns) {
                    if (it.first < g[y][x]) {
                        beforeCount++;
                    }
                }

                // Add to pattern
                patterns[g[y][x]] = beforeCount;
                invp[g[y][x]] = {x, y};
            }
        }
    }

    int j = WIDTH * HEIGHT;
    deltas = std::vector<int>(j, 0);
    int i = 0;
    for (auto it : patterns) {
        id *= j--;
        auto point = invp[it.first];
        int pos = point.y * WIDTH + point.x;
        id += pos - it.second;

        // Calculate deltas
        deltas[it.first] =
            perm(WIDTH * HEIGHT - 1 - i, WIDTH * HEIGHT - patterns.size());
        tiles.push_back({point.x, point.y});

        i++;
    }

    // Calculate compressed grid
    for (int y = HEIGHT - 1; y >= 0; y--) {
        for (int x = WIDTH - 1; x >= 0; x--) {
            grid = grid * 16 + g[y][x];
        }
    }
}

int Pattern::perm(int n, int k) const {
    int ans = 1;
    while (n > k) {
        ans *= n--;
    }
    return ans;
}

int Pattern::getCell(int x, int y) const {
    int i = 4 * (y * WIDTH + x);
    return ((grid & (0xfull << i)) >> i);
}

void Pattern::setCell(int x, int y, int n) {
    int i = 4 * (y * WIDTH + x);
    grid = (grid & ~(0xfull << i)) | ((uint64_t)n << i);
}

int Pattern::size() {
    return perm(WIDTH * HEIGHT, WIDTH * HEIGHT - tiles.size());
}

bool Pattern::canShift(int index, Direction dir) {
    auto t = tiles[index];
    int cellX = t.x;
    int cellY = t.y;

    switch (dir) {
        case Direction::U:
            return (cellY > 0 && getCell(cellX, cellY - 1) == 0);
        case Direction::R:
            return (cellX < WIDTH - 1 && getCell(cellX + 1, cellY) == 0);
        case Direction::D:
            return (cellY < HEIGHT - 1 && getCell(cellX, cellY + 1) == 0);
        default:
            return (cellX > 0 && getCell(cellX - 1, cellY) == 0);
    }
}

// Assumes canShift(index, dir)
void Pattern::shiftCell(int index, Direction dir) {
    auto& t = tiles[index];
    int cellX = t.x;
    int cellY = t.y;
    int tile = getCell(cellX, cellY);  // Value of sliding tile
    setCell(cellX, cellY, 0);

    switch (dir) {
        case Direction::U: {
            setCell(cellX, cellY - 1, tile);
            t.y--;

            int numGreater = 0;
            int numBlanks = 1;
            int skipDelta = 0;
            for (int x = cellX + 1; x < WIDTH; x++) {
                int skip = getCell(x, cellY - 1);
                if (skip == 0)
                    numBlanks++;
                else if (skip > tile) {
                    numGreater++;
                    skipDelta += deltas[skip];
                }
            }
            for (int x = 0; x < cellX; x++) {
                int skip = getCell(x, cellY);
                if (skip == 0)
                    numBlanks++;
                else if (skip > tile) {
                    numGreater++;
                    skipDelta += deltas[skip];
                }
            }
            id -= skipDelta + (numGreater + numBlanks) * deltas[tile];
            return;
        }
        case Direction::R:
            setCell(cellX + 1, cellY, tile);
            t.x++;

            id += deltas[tile];
            return;
        case Direction::D: {
            setCell(cellX, cellY + 1, tile);
            t.y++;

            int numGreater = 0;
            int numBlanks = 1;
            int skipDelta = 0;
            for (int x = cellX + 1; x < WIDTH; x++) {
                int skip = getCell(x, cellY);
                if (skip == 0)
                    numBlanks++;
                else if (skip > tile) {
                    numGreater++;
                    skipDelta += deltas[skip];
                }
            }
            for (int x = 0; x < cellX; x++) {
                int skip = getCell(x, cellY + 1);
                if (skip == 0)
                    numBlanks++;
                else if (skip > tile) {
                    numGreater++;
                    skipDelta += deltas[skip];
                }
            }
            id += skipDelta + (numGreater + numBlanks) * deltas[tile];
            return;
        }
        default:
            setCell(cellX - 1, cellY, tile);
            t.x--;

            id -= deltas[tile];
            return;
    }
}

Pattern::~Pattern() {}

std::ostream& operator<<(std::ostream& out, const Pattern& pattern) {
    out << "ID: " << pattern.id << std::endl;
    for (int y = 0; y < pattern.HEIGHT; y++) {
        for (int x = 0; x < pattern.WIDTH; x++) {
            out << std::setw(3) << pattern.getCell(x, y);
        }
        out << std::endl;
    }
    return out;
}
