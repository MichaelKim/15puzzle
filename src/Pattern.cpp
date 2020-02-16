#include "../include/Pattern.h"

int Pattern::getCell(int posn) const {
    int i = posn << 2;
    return (g & (0xfULL << i)) >> i;
}

void Pattern::setCell(int posn, int tile) {
    int i = posn << 2;
    g = (g & ~(0xfULL << i)) | ((uint64_t)tile << i);
}

bool Pattern::canShift(int tile, Direction dir) const {
    auto posn = pos[tile];

    switch (dir) {
        case Direction::U:
            return (posn >= WIDTH && getCell(posn - WIDTH) == 0);
        case Direction::R:
            return (posn % WIDTH < WIDTH - 1 && getCell(posn + 1) == 0);
        case Direction::D:
            return (posn < WIDTH * (HEIGHT - 1) && getCell(posn + WIDTH) == 0);
        default:
            return (posn % WIDTH > 0 && getCell(posn - 1) == 0);
    }
}

Pattern Pattern::shiftCell(int tile, Direction dir,
                           const std::vector<int>& deltas) const {
    Pattern next{id, dist + 1, pos, g, WIDTH, HEIGHT};
    auto& posn = next.pos[tile];
    next.setCell(posn, 0);

    switch (dir) {
        case Direction::U: {
            next.setCell(posn - WIDTH, tile);
            posn -= WIDTH;

            int numGreater = 0;
            int numBlanks = 1;
            int skipDelta = 0;
            for (uint i = posn + 1; i < posn + WIDTH; i++) {
                int skip = next.getCell(i);
                if (skip == 0) {
                    numBlanks++;
                } else if (skip > tile) {
                    numGreater++;
                    skipDelta += deltas[skip];
                }
            }
            next.id -= skipDelta + (numGreater + numBlanks) * deltas[tile];
            break;
        }
        case Direction::R:
            next.setCell(posn + 1, tile);
            posn++;

            next.id += deltas[tile];
            break;
        case Direction::D: {
            next.setCell(posn + WIDTH, tile);
            posn += WIDTH;

            int numGreater = 0;
            int numBlanks = 1;
            int skipDelta = 0;
            for (uint i = posn - WIDTH + 1; i < posn; i++) {
                int skip = getCell(i);
                if (skip == 0) {
                    numBlanks++;
                } else if (skip > tile) {
                    numGreater++;
                    skipDelta += deltas[skip];
                }
            }
            next.id += skipDelta + (numGreater + numBlanks) * deltas[tile];
            break;
        }
        default:
            next.setCell(posn - 1, tile);
            posn--;

            next.id -= deltas[tile];
            break;
    }

    return next;
}