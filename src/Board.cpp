#include "../include/Board.h"

#include <iomanip>
#include <iostream>

using namespace std;

const vector<vector<int>> Board::moves = {
  {0}, {1}, {2}, {3},
  {0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3},
  {0, 1, 2}, {0, 1, 3}, {0, 2, 3}, {1, 2, 3}
};

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
    // i = 4 * ((SIZE - y - 1) * SIZE + (SIZE - x - 1))
    int i = 4 * (SIZE * (SIZE - y) - x - 1);
    return ((grid & (0xfull << i)) >> i);
}

void Board::setCell(int x, int y, int n) {
    int i = 4 * (SIZE * (SIZE - y) - x - 1);
    grid = (grid & ~(0xfull << i)) | ((uint64_t) n << i);
}

uint64_t Board::getId() const {
    // Set blank point to 0
    return grid & ~(0xfull << (4 * (SIZE * (SIZE - blank.y) - blank.x - 1)));
}

Point Board::getBlank() {
    return blank;
}

const vector<int>& Board::getMoves(int prevMove) {
    if (blank.y == 0) {
        if (blank.x == 0) {
            if (prevMove == 3) return moves[2]; // 2
            if (prevMove == 0) return moves[1]; // 1
            return moves[7]; // 1, 2
        }
        if (blank.x == SIZE - 1) {
            if (prevMove == 0) return moves[3]; // 3
            if (prevMove == 1) return moves[2]; // 2
            return moves[9]; // 2, 3
        }
        if (prevMove == 3) return moves[9]; // 2, 3
        if (prevMove == 0) return moves[8]; // 1, 3
        if (prevMove == 1) return moves[7]; // 1, 2
        return moves[13]; // 1, 2, 3
    }
    if (blank.y == SIZE - 1) {
        if (blank.x == 0) {
            if (prevMove == 2) return moves[1]; // 1
            if (prevMove == 3) return moves[0]; // 0
            return moves[4]; // 0, 1
        }
        if (blank.x == SIZE - 1) {
            if (prevMove == 2) return moves[3]; // 3;
            if (prevMove == 1) return moves[0]; // 0
            return moves[6]; // 0, 3
        }
        if (prevMove == 2) return moves[8]; // 1, 3
        if (prevMove == 3) return moves[6]; // 0, 3
        if (prevMove == 1) return moves[4]; // 0, 1
        return moves[11]; // 0, 1, 3
    }
    if (blank.x == 0) {
        if (prevMove == 2) return moves[7]; // 1, 2
        if (prevMove == 3) return moves[5]; // 0, 2
        if (prevMove == 0) return moves[4]; // 0, 1
        return moves[10]; // 0, 1, 2
    }
    if (blank.x == SIZE - 1) {
        if (prevMove == 2) return moves[9]; // 2, 3
        if (prevMove == 0) return moves[6]; // 0, 3
        if (prevMove == 1) return moves[5]; // 0, 2
        return moves[12]; // 0, 2, 3
    }
    if (prevMove == 0) return moves[11]; // 0, 1, 3
    if (prevMove == 1) return moves[10]; // 0, 1, 2
    if (prevMove == 2) return moves[13]; // 1, 2, 3
    return moves[12]; // 0, 2, 3
}

void Board::applyMove(int dir) {
    if (dir == 0) {
        setCell(blank.x, blank.y, getCell(blank.x, blank.y - 1));
        blank.y -= 1;
    }
    else if (dir == 1) {
        setCell(blank.x, blank.y, getCell(blank.x + 1, blank.y));
        blank.x += 1;
    }
    else if (dir == 2) {
        setCell(blank.x, blank.y, getCell(blank.x, blank.y + 1));
        blank.y += 1;
    }
    else {
        setCell(blank.x, blank.y, getCell(blank.x - 1, blank.y));
        blank.x -= 1;
    }
}

void Board::undoMove(int dir) {
    if (dir == 0) {
        setCell(blank.x, blank.y, getCell(blank.x, blank.y + 1));
        blank.y += 1;
    }
    else if (dir == 1) {
        setCell(blank.x, blank.y, getCell(blank.x - 1, blank.y));
        blank.x -= 1;
    }
    else if (dir == 2) {
        setCell(blank.x, blank.y, getCell(blank.x, blank.y - 1));
        blank.y -= 1;
    }
    else {
        setCell(blank.x, blank.y, getCell(blank.x + 1, blank.y));
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
