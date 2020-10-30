#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <ostream>
#include <vector>

#include "Direction.h"
#include "DisjointDatabase.h"

class Board {
    const int WIDTH;
    const int HEIGHT;

    // {0, -1}, {1, 0}, {0, 1}, {-1, 0}}
    const std::array<int, 4> deltas;  // Blank deltas

    // Faster than computing on the fly
    const std::vector<std::array<bool, 4>> canMoveList;

    // Tiles
    int blank;  // Position of blank (since patterns don't store the blank)
    std::vector<int> grid;      // Value to position mapping
    std::vector<int> mirrGrid;  // Mirrored grid

    // Used for disjoint database
    std::vector<uint64_t> patterns;      // Pattern IDs
    std::vector<uint64_t> mirrPatterns;  // Mirrored pattern IDs

    // Used for walking distance
    int wdRowIndex;  // Chunk by row (1 2 3 4 / ...)
    int wdColIndex;  // Chunk by col (1 5 8 13 / ...)

    int getTile(int posn) const;
    void setTile(int posn, int tile);
    int getMirrTile(int posn) const;
    void setMirrTile(int posn, int tile);

    int getDelta(const std::vector<int>& g, int tile, int offset) const;

    struct MoveState {
        uint64_t pattern;
        uint64_t mirrPattern;
        int rowIndex;
        int colIndex;
        int blank;
    };

public:
    Board(const std::vector<int>& g, int width, int height);

    int getHeuristic() const;
    bool canMove(Direction dir);
    // Should be run only once at start of search
    std::vector<Direction> getMoves() const;
    MoveState applyMove(Direction dir);
    void undoMove(const MoveState& prev);

    friend std::ostream& operator<<(std::ostream& out, const Board& board);
};

#endif  // BOARD_H
