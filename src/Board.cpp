#include "../include/Board.h"

#include <iomanip>
#include <iostream>

using namespace std;

Board::Board(vector<vector<int>> g) {
    grid = 0;
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (g[y][x] == 0) {
                blank = {x, y};
            }
            grid = grid * 16 + g[y][x];
        }
    }
}

int Board::getCell(int x, int y) const {
    int i = 4 * ((SIZE - y - 1) * SIZE + (SIZE - x - 1));
    return ((grid & (0xfull << i)) >> i);
}

void Board::setCell(int x, int y, int n) {
    int i = 4 * ((SIZE - y - 1) * SIZE + (SIZE - x - 1));
    grid &= ~(0xfull << i);
    grid |= (ID) n << i;
}

ID Board::getId() const {
    return grid;
}

Point Board::getBlank() {
    return blank;
}

vector<int> Board::getMoves() {
    if (blank.y == 0) {
        if (blank.x == 0) return {1, 2};
        if (blank.x == SIZE - 1) return {2, 3};
        return {1, 2, 3};
    }
    if (blank.y == SIZE - 1) {
        if (blank.x == 0) return {0, 1};
        if (blank.x == SIZE - 1) return {0, 3};
        return {0, 1, 3};
    }
    if (blank.x == 0) return {0, 1, 2};
    if (blank.x == SIZE - 1) return {0, 2, 3};
    return {0, 1, 2, 3};
}

void Board::applyMove(int dir) {
    if (dir == 0) {
        setCell(blank.x, blank.y, getCell(blank.x, blank.y - 1));
        setCell(blank.x, blank.y - 1, 0);

        blank.y -= 1;
    }
    else if (dir == 1) {
        setCell(blank.x, blank.y, getCell(blank.x + 1, blank.y));
        setCell(blank.x + 1, blank.y, 0);

        blank.x += 1;
    }
    else if (dir == 2) {
        setCell(blank.x, blank.y, getCell(blank.x, blank.y + 1));
        setCell(blank.x, blank.y + 1, 0);

        blank.y += 1;
    }
    else {
        setCell(blank.x, blank.y, getCell(blank.x - 1, blank.y));
        setCell(blank.x - 1, blank.y, 0);

        blank.x -= 1;
    }
}

void Board::undoMove(int dir) {
    if (dir == 0) {
        setCell(blank.x, blank.y, getCell(blank.x, blank.y + 1));
        setCell(blank.x, blank.y + 1, 0);

        blank.y += 1;
    }
    else if (dir == 1) {
        setCell(blank.x, blank.y, getCell(blank.x - 1, blank.y));
        setCell(blank.x - 1, blank.y, 0);

        blank.x -= 1;
    }
    else if (dir == 2) {
        setCell(blank.x, blank.y, getCell(blank.x, blank.y - 1));
        setCell(blank.x, blank.y - 1, 0);

        blank.y -= 1;
    }
    else {
        setCell(blank.x, blank.y, getCell(blank.x + 1, blank.y));
        setCell(blank.x + 1, blank.y, 0);

        blank.x += 1;
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
