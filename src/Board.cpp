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

vector<int> Board::getMoves() {
    vector<int> moves;
    if (blank.y > 0) moves.push_back(0);
    if (blank.x < SIZE - 1) moves.push_back(1);
    if (blank.y < SIZE - 1) moves.push_back(2);
    if (blank.x > 0) moves.push_back(3);
    return moves;
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
