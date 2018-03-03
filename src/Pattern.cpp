#include "../include/Pattern.h"

#include <iomanip>
#include <iostream>
#include <ostream>
#include <vector>

Pattern::Pattern(std::vector<std::vector<int>> g) {
    grid = 0;
    for (int y = SIZE - 1; y >= 0; y--) {
        for (int x = SIZE - 1; x >= 0; x--) {
            if (g[y][x] > 0) {
                cells.push_back({x, y});
            }
            grid = grid * 16 + g[y][x];
        }
    }
}

int Pattern::getCell(int x, int y) const {
    int i = 4 * (y * SIZE + x);
    return ((grid & (0xfull << i)) >> i);
}

void Pattern::setCell(int x, int y, int n) {
    int i = 4 * (y * SIZE + x);
    grid = (grid & ~(0xfull << i)) | ((uint64_t)n << i);
}

uint64_t Pattern::getId() {
    return grid;
}

bool Pattern::canShift(int index, Direction dir) {
    int cellX = cells[index].x;
    int cellY = cells[index].y;

    if (dir == Direction::N) {
        return (cellY > 0 && getCell(cellX, cellY - 1) == 0);
    }
    else if (dir == Direction::E) {
        return (cellX < SIZE - 1 && getCell(cellX + 1, cellY) == 0);
    }
    else if (dir == Direction::S) {
        return (cellY < SIZE - 1 && getCell(cellX, cellY + 1) == 0);
    }
    else {
        return (cellX > 0 && getCell(cellX - 1, cellY) == 0);
    }
}

uint64_t Pattern::getShiftId(int index, Direction dir) {
    if (!canShift(index, dir)) return 0;

    int cellX = cells[index].x;
    int cellY = cells[index].y;

    int i = 4 * (cellY * SIZE + cellX);
    int j = 0;

    switch (dir) {
        case Direction::N:
            j = 4 * ((cellY - 1) * SIZE + cellX);
            break;
        case Direction::E:
            j = 4 * (cellY * SIZE + (cellX + 1));
            break;
        case Direction::S:
            j = 4 * ((cellY + 1) * SIZE + cellX);
            break;
        case Direction::W:
            j = 4 * (cellY * SIZE + (cellX - 1));
            break;
        default:
            return 0;
    }

    int temp = (grid & (0xfull << i)) >> i;
    return (grid & ~(0xfull << i) & ~(0xfull << j)) | ((uint64_t)temp << j);
}

// Assumes canShift(index, dir)
void Pattern::shiftCell(int index, Direction dir) {
    int cellX = cells[index].x;
    int cellY = cells[index].y;
    int temp = getCell(cellX, cellY);
    setCell(cellX, cellY, 0);

    switch (dir) {
        case Direction::N:
            setCell(cellX, cellY - 1, temp);
            cells[index].y--;
            break;
        case Direction::E:
            setCell(cellX + 1, cellY, temp);
            cells[index].x++;
            break;
        case Direction::S:
            setCell(cellX, cellY + 1, temp);
            cells[index].y++;
            break;
        case Direction::W:
            setCell(cellX - 1, cellY, temp);
            cells[index].x--;
            break;
        default:
            break;
    }
}

Pattern::~Pattern() {}

std::ostream& operator<<(std::ostream& out, const Pattern& pattern) {
    for (int y = 0; y < Pattern::SIZE; y++) {
        for (int x = 0; x < Pattern::SIZE; x++) {
            out << std::setw(3) << pattern.getCell(x, y);
        }
        out << std::endl;
    }
    return out;
}
