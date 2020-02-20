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

Pattern Pattern::shiftCell(int tile, Direction dir) const {
    Pattern next{dist + 1, pos, g, WIDTH, HEIGHT};
    auto& posn = next.pos[tile];
    next.setCell(posn, 0);

    switch (dir) {
        case Direction::U:
            next.setCell(posn - WIDTH, tile);
            posn -= WIDTH;
            break;
        case Direction::R:
            next.setCell(posn + 1, tile);
            posn++;
            break;
        case Direction::D:
            next.setCell(posn + WIDTH, tile);
            posn += WIDTH;
            break;

        default:
            next.setCell(posn - 1, tile);
            posn--;
            break;
    }

    return next;
}