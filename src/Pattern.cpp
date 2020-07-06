#include "../include/Pattern.h"

#include <unordered_map>

PatternGroup::PatternGroup(std::vector<std::vector<unsigned>> grid,
                           const unsigned WIDTH, const unsigned HEIGHT)
    : deltas(WIDTH * HEIGHT, 1),
      WIDTH(WIDTH),
      HEIGHT(HEIGHT),
      id(0),
      pos(WIDTH * HEIGHT, 0),
      g(0) {
    // Calculate compressed grid
    for (int y = HEIGHT; y--;) {
        for (int x = WIDTH; x--;) {
            g = (g << 4) + grid[y][x];
        }
    }

    // Calculate starting ID and starting position
    std::unordered_map<int, int> before;
    std::vector<unsigned> tiles;

    for (unsigned y = 0; y < HEIGHT; y++) {
        for (unsigned x = 0; x < WIDTH; x++) {
            if (grid[y][x] > 0) {
                // New tile found
                unsigned beforeCount = 0;

                // Count number of preceding pattern tiles that's smaller
                for (auto& it : before) {
                    if (it.first < grid[y][x]) {
                        beforeCount++;
                    }
                }

                before[grid[y][x]] = beforeCount;
                pos[grid[y][x]] = y * WIDTH + x;

                // Store tile
                tiles.push_back(grid[y][x]);
            }
        }
    }

    // Calculate deltas
    for (int i = tiles.size() - 1; i--;) {
        deltas[tiles[i]] = deltas[tiles[i + 1]] * (WIDTH * HEIGHT - 1 - i);
    }

    unsigned j = WIDTH * HEIGHT;
    for (auto tile : tiles) {
        id *= j--;
        id += pos[tile] - before[tile];
    }
}

Pattern PatternGroup::getPattern() { return {id, 0, pos, g}; }

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
    next.dist += 1;
    // Clear tile
    setCell(next, posn, 0);

    switch (dir) {
        case Direction::U: {
            setCell(next, posn - WIDTH, tile);
            posn -= WIDTH;

            int numDelta = 1;
            int skipDelta = 0;
            for (unsigned i = posn + 1; i < posn + WIDTH; i++) {
                auto skip = getCell(next, i);
                if (skip == 0) {
                    numDelta++;
                } else if (skip > tile) {
                    numDelta++;
                    skipDelta += deltas[skip];
                }
            }
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
            posn += WIDTH;

            int numDelta = 1;
            int skipDelta = 0;
            for (unsigned i = posn - WIDTH + 1; i < posn; i++) {
                auto skip = getCell(next, i);
                if (skip == 0) {
                    numDelta++;
                } else if (skip > tile) {
                    numDelta++;
                    skipDelta += deltas[skip];
                }
            }
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