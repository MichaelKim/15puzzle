#include "../include/Pattern.h"

#include <execution>
#include <numeric>
#include <unordered_map>

PatternGroup::PatternGroup(const std::vector<int>& grid, int width, int height)
    : WIDTH(width), HEIGHT(height), deltas(width * height, 1) {
    auto length = width * height;

    // Calculate starting ID and starting position
    std::unordered_map<int, int> before;
    std::vector<int> pos(length, 0);

    for (int i = 0; i < length; i++) {
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
    int j = length;
    int id = 0;
    for (auto tile : tiles) {
        id *= j--;
        id += pos[tile] - before[tile];
    }

    initPattern = {pos, grid, id};

    // Calculate deltas
    for (int i = tiles.size() - 1; i--;) {
        deltas[tiles[i]] = deltas[tiles[i + 1]] * (length - 1 - i);
    }
}

int PatternGroup::getCell(const Pattern& pattern, int position) const {
    return pattern.grid[position];
}

void PatternGroup::setCell(Pattern& pattern, int position, int tile) {
    pattern.grid[position] = tile;
}

bool PatternGroup::canShift(const Pattern& pattern, int tile,
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

int PatternGroup::getDelta(const Pattern& pattern, int tile, int offset) const {
    return std::transform_reduce(
        std::execution::par_unseq, pattern.grid.cbegin() + offset + 1,
        pattern.grid.cbegin() + offset + WIDTH, deltas[tile], std::plus<>(),
        [this, &tile](const auto skip) {
            if (skip == 0) {
                return deltas[tile];
            } else if (skip > tile) {
                return deltas[tile] + deltas[skip];
            } else {
                return 0;
            }
        });
}

Pattern PatternGroup::shiftCell(Pattern next, int tile, Direction dir) {
    // Position of tile
    auto& posn = next.pos[tile];
    // Clear tile
    setCell(next, posn, 0);

    switch (dir) {
        case Direction::U: {
            posn -= WIDTH;
            setCell(next, posn, tile);
            next.id -= getDelta(next, tile, posn);
            break;
        }
        case Direction::R:
            posn++;
            setCell(next, posn, tile);
            next.id += deltas[tile];
            break;
        case Direction::D: {
            posn += WIDTH;
            setCell(next, posn, tile);
            next.id += getDelta(next, tile, posn - WIDTH);
            break;
        }
        case Direction::L:
            posn--;
            setCell(next, posn, tile);
            next.id -= deltas[tile];
            break;
    }

    return next;
}