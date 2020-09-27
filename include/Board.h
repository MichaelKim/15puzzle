#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <ostream>
#include <vector>

#include "Direction.h"
#include "DisjointDatabase.h"

class Board {
private:
    const int WIDTH = 4, HEIGHT = 4;

    int blank;  // Position of blank (since patterns don't store the blank)
    // {0, -1}, {1, 0}, {0, 1}, {-1, 0}}
    const std::array<int, 4> deltas = {-WIDTH, 1, WIDTH, -1};  // Blank deltas
    std::array<int, 16> grid;              // Value to position mapping
    std::array<int, 16> mirrGrid;          // Mirrored grid
    std::array<int, 16> mirror = [this] {  // Mirror position
        // Calculate mirror positions
        // TODO: test with blank not in top-left or bottom-right
        std::array<int, 16> mirror;
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            int y = i / WIDTH;
            int x = i % WIDTH;
            mirror[i] = (x * WIDTH) + y;
        }
        return mirror;
    }();

    // Used for disjoint database
    const DisjointDatabase& database;    // Pattern heuristic
    std::array<int, 16> tileDeltas;      // Tile deltas
    std::vector<uint64_t> patterns;      // Pattern IDs
    std::vector<uint64_t> mirrPatterns;  // Mirrored pattern IDs

    // Used for walking distance
    int wdRowIndex;  // Chunk by row (1 2 3 4 / ...)
    int wdColIndex;  // Chunk by col (1 5 8 13 / ...)

    // Faster than computing on the fly
    std::array<std::array<bool, 4>, 16> canMoveList = [this] {
        std::array<std::array<bool, 4>, 16> moves{};

        // Blank position
        for (int i = 0; i < 16; i++) {
            moves[i][static_cast<int>(Direction::U)] = (i / 4) > 0;
            moves[i][static_cast<int>(Direction::R)] = (i % 4) < WIDTH - 1;
            moves[i][static_cast<int>(Direction::D)] = (i / 4) < HEIGHT - 1;
            moves[i][static_cast<int>(Direction::L)] = (i % 4) > 0;
        }

        return moves;
    }();

    std::vector<uint64_t> generatePatterns(
        const std::array<int, 16>& grid,
        const std::vector<std::vector<int>>& patternTiles);

    int getTile(int posn) const;
    void setTile(int posn, int tile);
    int getMirrTile(int posn) const;
    void setMirrTile(int posn, int tile);

    struct MoveState {
        uint64_t pattern;
        uint64_t mirrPattern;
        int rowIndex;
        int colIndex;
        int blank;
    };

    std::array<int, 16> calculateDeltas(
        const std::vector<std::vector<int>>& patternTiles);

public:
    Board(const std::array<int, 16>& g, const DisjointDatabase& d);

    int getHeuristic() const;
    bool canMove(Direction dir);
    std::vector<Direction> getMoves() const;
    MoveState applyMove(Direction dir);
    void undoMove(const MoveState& prev);

    friend std::ostream& operator<<(std::ostream& out, const Board& board);
};

#endif  // BOARD_H
