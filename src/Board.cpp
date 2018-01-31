#include "../include/Board.h"

#include <iomanip>
#include <iostream>

using namespace std;

Board::Board(vector<vector<int>> g) {
    grid = 0;
    for (int y = SIZE - 1; y >= 0; y--) {
        for (int x = SIZE - 1; x >= 0; x--) {
            if (g[y][x] == 0) {
                blank = {x, y};
            }
            grid = grid * 16 + g[y][x];
        }
    }
}

Board::Board(ID val): grid(val) {
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            int i = 4 * (y * SIZE + x);
            if (((val & (0xfull << i)) >> i) == 0) {
                blank = {x, y};
                return;
            }
        }
    }
}

int Board::getCell(int x, int y) const {
    int i = 4 * (y * SIZE + x);
    return ((grid & (0xfull << i)) >> i);
}

void Board::setCell(int x, int y, int n) {
    int i = 4 * (y * SIZE + x);
    grid &= ~(0xfull << i);
    grid |= (ID) n << i;
}

ID Board::getId() {
    return grid;
}

Point Board::getBlank() {
    return blank;
}

bool Board::canShiftBlank(Direction dir) {
    if (dir == Direction::N) {
        return blank.y > 0;
    }
    else if (dir == Direction::E) {
        return blank.x < SIZE - 1;
    }
    else if (dir == Direction::S) {
        return blank.y < SIZE - 1;
    }
    else {
        return blank.x > 0;
    }
}

void Board::shiftBlank(Direction dir) {
    int temp = getCell(blank.x, blank.y);
    if (dir == Direction::N) {
        setCell(blank.x, blank.y, getCell(blank.x, blank.y - 1));
        setCell(blank.x, blank.y - 1, temp);

        blank.y -= 1;
    }
    else if (dir == Direction::E) {
        setCell(blank.x, blank.y, getCell(blank.x + 1, blank.y));
        setCell(blank.x + 1, blank.y, temp);

        blank.x += 1;
    }
    else if (dir == Direction::S) {
        setCell(blank.x, blank.y, getCell(blank.x, blank.y + 1));
        setCell(blank.x, blank.y + 1, temp);

        blank.y += 1;
    }
    else {
        setCell(blank.x, blank.y, getCell(blank.x - 1, blank.y));
        setCell(blank.x - 1, blank.y, temp);

        blank.x -= 1;
    }
}

Board::~Board() {}

ostream& operator<<(ostream& out, const Board& board) {
    for (int y = 0; y < Board::SIZE; y++) {
        for (int x = 0; x < Board::SIZE; x++) {
            out << setw(3) << board.getCell(x, y);
        }
        out << endl;
    }
    return out;
}
