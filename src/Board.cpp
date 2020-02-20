#include "../include/Board.h"

#include <iomanip>
#include <iostream>
#include <unordered_map>

const std::vector<Direction>& Board::generateMoveList(int x, int y,
                                                      Direction prevMove) {
    if (y == 0) {
        if (x == 0) {
            if (prevMove == Direction::L) return moves[2];  // 2
            if (prevMove == Direction::U) return moves[1];  // 1
            return moves[7];                                // 1, 2
        }
        if (x == WIDTH - 1) {
            if (prevMove == Direction::U) return moves[3];  // 3
            if (prevMove == Direction::R) return moves[2];  // 2
            return moves[9];                                // 2, 3
        }
        if (prevMove == Direction::L) return moves[9];  // 2, 3
        if (prevMove == Direction::U) return moves[8];  // 1, 3
        if (prevMove == Direction::R) return moves[7];  // 1, 2
        return moves[13];                               // 1, 2, 3
    }
    if (y == HEIGHT - 1) {
        if (x == 0) {
            if (prevMove == Direction::D) return moves[1];  // 1
            if (prevMove == Direction::L) return moves[0];  // 0
            return moves[4];                                // 0, 1
        }
        if (x == WIDTH - 1) {
            if (prevMove == Direction::D) return moves[3];  // 3;
            if (prevMove == Direction::R) return moves[0];  // 0
            return moves[6];                                // 0, 3
        }
        if (prevMove == Direction::D) return moves[8];  // 1, 3
        if (prevMove == Direction::L) return moves[6];  // 0, 3
        if (prevMove == Direction::R) return moves[4];  // 0, 1
        return moves[11];                               // 0, 1, 3
    }
    if (x == 0) {
        if (prevMove == Direction::D) return moves[7];  // 1, 2
        if (prevMove == Direction::L) return moves[5];  // 0, 2
        if (prevMove == Direction::U) return moves[4];  // 0, 1
        return moves[10];                               // 0, 1, 2
    }
    if (x == WIDTH - 1) {
        if (prevMove == Direction::D) return moves[9];  // 2, 3
        if (prevMove == Direction::U) return moves[6];  // 0, 3
        if (prevMove == Direction::R) return moves[5];  // 0, 2
        return moves[12];                               // 0, 2, 3
    }
    if (prevMove == Direction::U) return moves[11];  // 0, 1, 3
    if (prevMove == Direction::R) return moves[10];  // 0, 1, 2
    if (prevMove == Direction::D) return moves[13];  // 1, 2, 3
    if (prevMove == Direction::L) return moves[12];  // 0, 2, 3
    return moves[14];                                // 0, 1, 2, 3
}

Board::Board(const std::vector<std::vector<int>>& g, const DisjointDatabase& d)
    : blank(0), database(d), WIDTH(g[0].size()), HEIGHT(g.size()) {
    uint64_t positions = 0;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            positions |= (uint64_t)(y * WIDTH + x) << (4 * g[y][x]);
            grid.push_back(g[y][x]);

            if (g[y][x] == 0) blank = y * WIDTH + x;
        }
    }

    // x, y, prevMove, moves[]
    moveList = std::vector<std::vector<std::vector<Direction>>>(
        WIDTH * HEIGHT, std::vector<std::vector<Direction>>(5));

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            for (int m = 0; m < 4; m++) {
                moveList[y * WIDTH + x][m] =
                    generateMoveList(x, y, static_cast<Direction>(m));
            }
        }
    }

    deltas = std::vector<int>{-WIDTH, 1, WIDTH, -1};

    // Calculate partial positions
    auto numPatterns = database.numPatterns();
    const auto& where = database.where;
    partialPositions.resize(numPatterns, 0);

    for (int i = 0; i < where.size(); i++) {
        int index = where[i];
        int pos = (positions >> (4 * i)) & 0xf;
        partialPositions[index] |= (uint64_t)i << (4 * pos);
    }
}

int Board::getHeuristic() const {
    return database.getHeuristic(partialPositions);
}

const std::vector<Direction>& Board::getMoves() const {
    // Should be run only once at start of search
    if (blank < WIDTH) {           // top
        if (blank % WIDTH == 0) {  // left
            return moves[7];
        }
        if (blank % WIDTH == WIDTH - 1) {  // right
            return moves[9];
        }
        return moves[13];
    }
    if (blank >= (WIDTH - 1) * HEIGHT) {  // bottom
        if (blank % WIDTH == 0) {         // left
            return moves[4];
        }
        if (blank % WIDTH == WIDTH - 1) {  // right
            return moves[6];
        }
        return moves[11];
    }
    if (blank % WIDTH == 0) {  // left
        return moves[10];
    }
    if (blank % WIDTH == WIDTH - 1) {  // right
        return moves[12];
    }

    return moves[14];
}

const std::vector<Direction>& Board::getMoves(Direction prevMove) const {
    // Branchless lookup for moves
    return moveList[blank][static_cast<int>(prevMove)];
}

inline int Board::getTile(int posn) { return grid[posn]; }

inline void Board::setTile(int posn, int tile) { grid[posn] = tile; }

// Pattern ID, pattern index
void Board::applyMove(Direction dir) {
    // Position of blank
    const auto& delta = deltas[static_cast<int>(dir)];
    // Position of sliding tile
    const int slidingPos = blank + delta;
    // Value of sliding tile
    const int tile = getTile(slidingPos);

    // Set value of slid tile
    setTile(blank, tile);

    // Update pattern
    int index = database.where[tile];  // Which pattern the sliding tile is in

    // Set position of slid tile
    partialPositions[index] =
        (partialPositions[index] & ~(0xfull << (4 * blank))) |
        ((uint64_t)tile << (4 * blank));

    // Clear blank tile
    partialPositions[index] =
        partialPositions[index] & ~(0xfull << (4 * slidingPos));

    // Update blank tile
    blank += delta;
}

void Board::undoMove(Direction dir) {
    // Position of blank
    const auto& delta = deltas[static_cast<int>(dir)];
    // Position of sliding tile
    const int slidingPos = blank - delta;
    // Value of sliding tile
    const int tile = getTile(slidingPos);

    // Set value of slid tile
    setTile(blank, tile);

    // Update pattern
    int index = database.where[tile];  // Which pattern the sliding tile is in

    // Set position of slid tile
    partialPositions[index] =
        (partialPositions[index] & ~(0xfull << (4 * blank))) |
        ((uint64_t)tile << (4 * blank));

    // Clear blank tile
    partialPositions[index] =
        partialPositions[index] & ~(0xfull << (4 * slidingPos));

    // Update blank tile
    blank -= delta;
}

Board::~Board() = default;

std::ostream& operator<<(std::ostream& out, const Board& board) {
    for (int y = 0; y < board.HEIGHT; y++) {
        for (int x = 0; x < board.WIDTH; x++) {
            const int i = y * board.WIDTH + x;
            if (i == board.blank) {
                out << std::setw(3) << 0;
            } else {
                out << std::setw(3) << board.grid[i];
            }
        }
        out << std::endl;
    }
    return out;
}
