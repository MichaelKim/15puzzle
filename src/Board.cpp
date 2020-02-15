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

Board::Board(std::vector<std::vector<int>> g, const DisjointDatabase& d)
    : blank(0), grid(0), database(d), WIDTH(g[0].size()), HEIGHT(g.size()) {
    uint64_t positions = 0;
    for (int y = HEIGHT - 1; y >= 0; y--) {
        for (int x = WIDTH - 1; x >= 0; x--) {
            positions |= (uint64_t)(y * WIDTH + x) << (4 * g[y][x]);
            grid = grid * 16 + g[y][x];

            if (g[y][x] == 0) blank = y * WIDTH + x;
        }
    }

    // x, y, prevMove, moves[]
    moveList = std::vector<std::vector<std::vector<Move>>>(
        WIDTH * HEIGHT, std::vector<std::vector<Move>>(5));

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            for (int m = 0; m < 5; m++) {
                moveList[y * WIDTH + x][m] =
                    generateMoveList(x, y, static_cast<Move>(m));
            }
        }
    }

    // {0, 0}, {0, -1}, {1, 0}, {0, 1}, {-1, 0}};
    deltas = std::vector<int>{0, -4 * WIDTH, 4, 4 * WIDTH, -4};

    // Calculate partial positions
    int numPatterns = database.numPatterns();
    const auto& where = database.where;
    partialPositions.resize(numPatterns, 0);

    for (size_t i = 0; i < where.size(); i++) {
        int index = where[i];
        int pos = (positions >> (4 * i)) & 0xf;
        partialPositions[index] |= (uint64_t)i << (4 * pos);
    }
}

int Board::getHeuristic() const {
    return database.getHeuristic(partialPositions);
}

const std::vector<Move>& Board::getMoves(Move prevMove) {
    // Branchless lookup for moves
    return moveList[blank][static_cast<int>(prevMove)];
}

int Board::applyMove(Move dir) {
    // Will never be NULL
    // Position of blank
    const auto& delta = deltas[static_cast<int>(dir)];
    const int blank = 4 * this->blank;
    // Position of sliding tile
    const int slidingPos = blank + delta;
    // Value of sliding tile
    const int slidingValue = (grid & (0xfull << slidingPos)) >> slidingPos;

    // Set value of slid tile
    grid = (grid & ~(0xfull << blank)) | ((uint64_t)slidingValue << blank);

    // Update partial position
    int index =
        database.where[slidingValue];  // Which pattern the sliding tile is in

    // Set position of slid tile
    partialPositions[index] = (partialPositions[index] & ~(0xfull << blank)) |
                              ((uint64_t)slidingValue << blank);

    // Clear blank tile
    partialPositions[index] = partialPositions[index] & ~(0xfull << slidingPos);

    // Update blank tile
    this->blank += delta / 4;

    return 1;
}

void Board::undoMove(Move dir) {
    // Will never be NULL
    // Position of blank
    const auto& delta = deltas[static_cast<int>(dir)];
    const int blank = 4 * this->blank;
    // Position of sliding tile
    const int slidingPos = blank - delta;
    // Value of sliding tile
    const int slidingValue = (grid & (0xfull << slidingPos)) >> slidingPos;

    // Set value of slid tile
    grid = (grid & ~(0xfull << blank)) | ((uint64_t)slidingValue << blank);

    // Update partial position
    int index =
        database.where[slidingValue];  // Which pattern the sliding tile is in

    // Set position of slid tile
    partialPositions[index] = (partialPositions[index] & ~(0xfull << blank)) |
                              ((uint64_t)slidingValue << blank);

    // Clear blank tile
    partialPositions[index] = partialPositions[index] & ~(0xfull << slidingPos);

    // Update blank tile
    this->blank -= delta / 4;
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
            const int i = y * board.WIDTH + x;
            if (i == board.blank) {
                out << std::setw(3) << 0;
            } else {
                out << std::setw(3)
                    << ((board.grid & (0xfull << (4 * i))) >> (4 * i));
            }
        }
        out << std::endl;
    }
    return out;
}
