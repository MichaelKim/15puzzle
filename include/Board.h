#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <ostream>
#include <vector>

#include "Direction.h"
#include "DisjointDatabase.h"
#include "WalkingDistance.h"

class Board {
private:
    const int WIDTH = 4, HEIGHT = 4;

    // Used for disjoint database
    int blank;  // Position of blank (since patterns don't store the blank)
    std::array<int, 16> grid;            // Value to position mapping
    std::array<int, 16> mirrGrid;        // Mirrored grid
    std::vector<uint64_t> patterns;      // Pattern IDs
    std::vector<uint64_t> mirrPatterns;  // Mirrored pattern IDs

    // Lookup tables
    const DisjointDatabase& database;  // Pattern heuristic
    // {0, -1}, {1, 0}, {0, 1}, {-1, 0}}
    const std::array<int, 4> deltas = {-WIDTH, 1, WIDTH, -1};  // Blank deltas
    std::array<int, 16> tileDeltas;                            // Tile deltas
    std::array<int, 16> mirror;  // Mirror position

    // Used for walking distance
    const WalkingDistance& wdDb;
    int wdRowIndex;  // Chunk by row (1 2 3 4 / ...)
    int wdColIndex;  // Chunk by col (1 5 8 13 / ...)

    constexpr std::array<std::array<bool, 4>, 16> initMoveList();
    std::array<std::array<bool, 4>, 16> canMoveList;

    std::vector<uint64_t> generatePatterns(
        const std::array<int, 16>& grid,
        const std::vector<std::vector<int>>& patternTiles);

    int getTile(int posn) const;
    void setTile(int posn, int tile);
    int getMirrTile(int posn) const;
    void setMirrTile(int posn, int tile);

public:
    Board(const std::array<int, 16>& g, const DisjointDatabase& d,
          const WalkingDistance& w);

    int getHeuristic() const;
    bool canMove(Direction dir);
    std::vector<Direction> getMoves() const;
    std::tuple<uint64_t, uint64_t, int, int> applyMove(Direction dir);
    void undoMove(const std::tuple<uint64_t, uint64_t, int, int>& prev,
                  Direction dir);

    friend std::ostream& operator<<(std::ostream& out, const Board& board);
};

#endif  // BOARD_H
