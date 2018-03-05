#include "../include/Board.h"

#include <iomanip>
#include <iostream>

using Move = Board::Move;

const std::vector<std::vector<Move>> Board::moves = {
    {Move::N},
    {Move::E},
    {Move::S},
    {Move::W},
    {Move::N, Move::E},
    {Move::N, Move::S},
    {Move::N, Move::W},
    {Move::E, Move::S},
    {Move::E, Move::W},
    {Move::S, Move::W},
    {Move::N, Move::E, Move::S},
    {Move::N, Move::E, Move::W},
    {Move::N, Move::S, Move::W},
    {Move::E, Move::S, Move::W}};

Board::Board(std::vector<std::vector<int>> g)
    : WIDTH(g[0].size()), HEIGHT(g.size()) {
    grid = 0;
    for (int y = HEIGHT - 1; y >= 0; y--) {
        for (int x = WIDTH - 1; x >= 0; x--) {
            if (g[y][x] == 0) {
                blank = {x, y};
            }
            grid = grid * 16 + g[y][x];
        }
    }
}

int Board::getCell(int x, int y) const {
    int i = 4 * (y * WIDTH + x);
    return ((grid & (0xfull << i)) >> i);
}

void Board::setCell(int x, int y, int n) {
    int i = 4 * (y * WIDTH + x);
    grid = (grid & ~(0xfull << i)) | ((uint64_t)n << i);
}

uint64_t Board::getId() const {
    // Set blank point to 0
    return grid & ~(0xfull << (4 * (blank.y * WIDTH + blank.x)));
}

Point Board::getBlank() {
    return blank;
}

const std::vector<Move>& Board::getMoves(Move prevMove) {
    if (blank.y == 0) {
        if (blank.x == 0) {
            if (prevMove == Move::W) return moves[2];  // 2
            if (prevMove == Move::N) return moves[1];  // 1
            return moves[7];                           // 1, 2
        }
        if (blank.x == WIDTH - 1) {
            if (prevMove == Move::N) return moves[3];  // 3
            if (prevMove == Move::E) return moves[2];  // 2
            return moves[9];                           // 2, 3
        }
        if (prevMove == Move::W) return moves[9];  // 2, 3
        if (prevMove == Move::N) return moves[8];  // 1, 3
        if (prevMove == Move::E) return moves[7];  // 1, 2
        return moves[13];                          // 1, 2, 3
    }
    if (blank.y == HEIGHT - 1) {
        if (blank.x == 0) {
            if (prevMove == Move::S) return moves[1];  // 1
            if (prevMove == Move::W) return moves[0];  // 0
            return moves[4];                           // 0, 1
        }
        if (blank.x == WIDTH - 1) {
            if (prevMove == Move::S) return moves[3];  // 3;
            if (prevMove == Move::E) return moves[0];  // 0
            return moves[6];                           // 0, 3
        }
        if (prevMove == Move::S) return moves[8];  // 1, 3
        if (prevMove == Move::W) return moves[6];  // 0, 3
        if (prevMove == Move::E) return moves[4];  // 0, 1
        return moves[11];                          // 0, 1, 3
    }
    if (blank.x == 0) {
        if (prevMove == Move::S) return moves[7];  // 1, 2
        if (prevMove == Move::W) return moves[5];  // 0, 2
        if (prevMove == Move::N) return moves[4];  // 0, 1
        return moves[10];                          // 0, 1, 2
    }
    if (blank.x == WIDTH - 1) {
        if (prevMove == Move::S) return moves[9];  // 2, 3
        if (prevMove == Move::N) return moves[6];  // 0, 3
        if (prevMove == Move::E) return moves[5];  // 0, 2
        return moves[12];                          // 0, 2, 3
    }
    if (prevMove == Move::N) return moves[11];  // 0, 1, 3
    if (prevMove == Move::E) return moves[10];  // 0, 1, 2
    if (prevMove == Move::S) return moves[13];  // 1, 2, 3
    return moves[12];                           // 0, 2, 3
}

void Board::applyMove(Move dir) {
    switch (dir) {
        case Move::N:
            setCell(blank.x, blank.y, getCell(blank.x, blank.y - 1));
            blank.y -= 1;
            return;
        case Move::E:
            setCell(blank.x, blank.y, getCell(blank.x + 1, blank.y));
            blank.x += 1;
            return;
        case Move::S:
            setCell(blank.x, blank.y, getCell(blank.x, blank.y + 1));
            blank.y += 1;
            return;
        case Move::W:
            setCell(blank.x, blank.y, getCell(blank.x - 1, blank.y));
            blank.x -= 1;
            return;
        default:
            return;
    }
}

void Board::undoMove(Move dir) {
    switch (dir) {
        case Move::N:
            setCell(blank.x, blank.y, getCell(blank.x, blank.y + 1));
            blank.y += 1;
            return;
        case Move::E:
            setCell(blank.x, blank.y, getCell(blank.x - 1, blank.y));
            blank.x -= 1;
            return;
        case Move::S:
            setCell(blank.x, blank.y, getCell(blank.x, blank.y - 1));
            blank.y -= 1;
            return;
        case Move::W:
            setCell(blank.x, blank.y, getCell(blank.x + 1, blank.y));
            blank.x += 1;
            return;
        default:
            return;
    }
}

Board::~Board() {}

std::ostream& operator<<(std::ostream& out, const Move& move) {
    switch (move) {
        case Move::N:
            out << "N";
            break;
        case Move::E:
            out << "E";
            break;
        case Move::S:
            out << "S";
            break;
        case Move::W:
            out << "W";
            break;
        default:
            break;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const Board& board) {
    for (int y = 0; y < board.HEIGHT; y++) {
        for (int x = 0; x < board.WIDTH; x++) {
            if (x == board.blank.x && y == board.blank.y) {
                out << std::setw(3) << 0;
            }
            else {
                out << std::setw(3) << board.getCell(x, y);
            }
        }
        out << std::endl;
    }
    return out;
}
