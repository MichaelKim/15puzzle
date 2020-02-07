#include "../include/Board.h"

#include <iomanip>
#include <iostream>

using Move = Board::Move;

const std::vector<Move>& Board::generateMoveList(int x, int y, Move prevMove) {
    if (y == 0) {
        if (x == 0) {
            if (prevMove == Move::L) return moves[2];  // 2
            if (prevMove == Move::U) return moves[1];  // 1
            return moves[7];                           // 1, 2
        }
        if (x == WIDTH - 1) {
            if (prevMove == Move::U) return moves[3];  // 3
            if (prevMove == Move::R) return moves[2];  // 2
            return moves[9];                           // 2, 3
        }
        if (prevMove == Move::L) return moves[9];  // 2, 3
        if (prevMove == Move::U) return moves[8];  // 1, 3
        if (prevMove == Move::R) return moves[7];  // 1, 2
        return moves[13];                          // 1, 2, 3
    }
    if (y == HEIGHT - 1) {
        if (x == 0) {
            if (prevMove == Move::D) return moves[1];  // 1
            if (prevMove == Move::L) return moves[0];  // 0
            return moves[4];                           // 0, 1
        }
        if (x == WIDTH - 1) {
            if (prevMove == Move::D) return moves[3];  // 3;
            if (prevMove == Move::R) return moves[0];  // 0
            return moves[6];                           // 0, 3
        }
        if (prevMove == Move::D) return moves[8];  // 1, 3
        if (prevMove == Move::L) return moves[6];  // 0, 3
        if (prevMove == Move::R) return moves[4];  // 0, 1
        return moves[11];                          // 0, 1, 3
    }
    if (x == 0) {
        if (prevMove == Move::D) return moves[7];  // 1, 2
        if (prevMove == Move::L) return moves[5];  // 0, 2
        if (prevMove == Move::U) return moves[4];  // 0, 1
        return moves[10];                          // 0, 1, 2
    }
    if (x == WIDTH - 1) {
        if (prevMove == Move::D) return moves[9];  // 2, 3
        if (prevMove == Move::U) return moves[6];  // 0, 3
        if (prevMove == Move::R) return moves[5];  // 0, 2
        return moves[12];                          // 0, 2, 3
    }
    if (prevMove == Move::U) return moves[11];  // 0, 1, 3
    if (prevMove == Move::R) return moves[10];  // 0, 1, 2
    if (prevMove == Move::D) return moves[13];  // 1, 2, 3
    if (prevMove == Move::L) return moves[12];  // 0, 2, 3
    return moves[14];                           // 0, 1, 2, 3
}

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

    // x, y, prevMove, moves[]
    moveList = std::vector<std::vector<std::vector<std::vector<Move>>>>(
        HEIGHT, std::vector<std::vector<std::vector<Move>>>(
                    WIDTH, std::vector<std::vector<Move>>(5)));

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            for (int m = 0; m < 5; m++) {
                moveList[y][x][m] =
                    generateMoveList(x, y, static_cast<Move>(m));
            }
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

Point Board::getBlank() { return blank; }

const std::vector<Move>& Board::getMoves(Move prevMove) {
    // Branchless lookup for moves
    return moveList[blank.y][blank.x][static_cast<int>(prevMove)];
}

int Board::applyMove(Move dir) {
    // Will never be NULL
    const auto& delta = deltas[static_cast<int>(dir)];
    setCell(blank.x, blank.y,
            getCell(blank.x + delta.first, blank.y + delta.second));
    blank.x += delta.first;
    blank.y += delta.second;
    return 1;
}

void Board::undoMove(Move dir) {
    // Will never be NULL
    const auto& delta = deltas[static_cast<int>(dir)];
    setCell(blank.x, blank.y,
            getCell(blank.x - delta.first, blank.y - delta.second));
    blank.x -= delta.first;
    blank.y -= delta.second;
}

Board::~Board() {}

std::ostream& operator<<(std::ostream& out, const Move& move) {
    switch (move) {
        case Move::U:
            out << "D";
            break;
        case Move::R:
            out << "L";
            break;
        case Move::D:
            out << "U";
            break;
        case Move::L:
            out << "R";
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
            } else {
                out << std::setw(3) << board.getCell(x, y);
            }
        }
        out << std::endl;
    }
    return out;
}
