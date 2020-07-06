#include "../include/Pattern.h"

#include <iostream>
#include <unordered_map>

PatternGroup::PatternGroup(const std::array<int, 16>& grid) {
    // Calculate compressed grid
    uint64_t g = 0;
    for (int i = WIDTH * HEIGHT; i--;) {
        g = (g << 4) + grid[i];
    }

    // Calculate starting ID and starting position
    std::unordered_map<int, int> before;
    std::vector<int> tiles;
    std::array<int, 16> pos;
    pos.fill(0);

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (grid[i] > 0) {
            // New tile found
            int beforeCount = 0;

            // Count number of preceding pattern tiles that's smaller
            for (auto& it : before) {
                if (it.first < grid[i]) {
                    beforeCount++;
                }
            }

            before[grid[i]] = beforeCount;
            pos[grid[i]] = i;

            // Store tile
            tiles.push_back(grid[i]);
        }
    }

    // Calculate id
    int j = WIDTH * HEIGHT;
    uint64_t id = 0;
    for (auto tile : tiles) {
        id *= j--;
        id += pos[tile] - before[tile];
    }

    initPattern = Pattern{pos, id, g};

    // Calculate deltas
    deltas.fill(1);
    for (int i = tiles.size() - 1; i--;) {
        deltas[tiles[i]] = deltas[tiles[i + 1]] * (WIDTH * HEIGHT - 1 - i);
    }
}

unsigned PatternGroup::getCell(const Pattern& pattern, int position) const {
    auto i = position << 2;
    return (pattern.g & (0xfULL << i)) >> i;
}

void PatternGroup::setCell(Pattern& pattern, int position, unsigned tile) {
    auto i = position << 2;
    pattern.g = (pattern.g & ~(0xfULL << i)) | ((uint64_t)tile << i);
}

bool PatternGroup::canShift(const Pattern& pattern, unsigned tile,
                            Direction dir) const {
    auto posn = pattern.pos[tile];

    switch (dir) {
        case Direction::U:
            return (posn >= WIDTH && getCell(pattern, posn - WIDTH) == 0);
        case Direction::R:
            return (posn % WIDTH < WIDTH - 1 &&
                    getCell(pattern, posn + 1) == 0);
        case Direction::D:
            return (posn < WIDTH * (HEIGHT - 1) &&
                    getCell(pattern, posn + WIDTH) == 0);
        default:
            return (posn % WIDTH > 0 && getCell(pattern, posn - 1) == 0);
    }
}

Pattern PatternGroup::shiftCell(Pattern next, unsigned tile, Direction dir) {
    // Position of tile
    auto& posn = next.pos[tile];
    // Clear tile
    setCell(next, posn, 0);

    switch (dir) {
        case Direction::U: {
            setCell(next, posn - WIDTH, tile);

            int numDelta = 1;
            int skipDelta = 0;
            for (int i = 1; i < WIDTH; i++) {
                auto skip = getCell(next, i + posn - WIDTH);
                if (skip == 0) {
                    numDelta++;
                } else if (skip > tile) {
                    numDelta++;
                    skipDelta += deltas[skip];
                }
            }

            posn -= WIDTH;
            next.id -= skipDelta + numDelta * deltas[tile];
            break;
        }
        case Direction::R:
            setCell(next, posn + 1, tile);

            posn++;
            next.id += deltas[tile];
            break;
        case Direction::D: {
            setCell(next, posn + WIDTH, tile);

            int numDelta = 1;
            int skipDelta = 0;
            for (int i = 1; i <= WIDTH; i++) {
                auto skip = getCell(next, i + posn);
                if (skip == 0) {
                    numDelta++;
                } else if (skip > tile) {
                    numDelta++;
                    skipDelta += deltas[skip];
                }
            }

            posn += WIDTH;
            next.id += skipDelta + numDelta * deltas[tile];
            break;
        }
        default:
            setCell(next, posn - 1, tile);

            posn--;
            next.id -= deltas[tile];
            break;
    }

    return next;
}