#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <ostream>
#include <vector>

#include "Direction.h"
#include "DisjointDatabase.h"

class Board {
private:
    int blank;  // Position of blank (since patterns don't store the blank)
    std::vector<int> grid;               // Value to position mapping
    std::vector<int> mirrGrid;           // Mirrored grid
    std::vector<uint64_t> patterns;      // Pattern IDs
    std::vector<uint64_t> mirrPatterns;  // Mirrored pattern IDs

    // Lookup tables
    const DisjointDatabase& database;  // Pattern heuristic
    std::vector<int> deltas;           // Blank deltas
    std::vector<int> tileDeltas;       // Tile deltas
    std::vector<int> mirror;           // Mirror position

    std::vector<std::array<bool, 4>> initMoveList(int WIDTH, int HEIGHT);
    std::vector<std::array<bool, 4>> canMoveList;

    std::vector<uint64_t> generatePatterns(
        const std::vector<int>& grid,
        const std::vector<std::vector<int>>& patternTiles);

    inline int getTile(int posn);
    inline void setTile(int posn, int tile);

public:
    const int WIDTH, HEIGHT;

    Board(const std::vector<std::vector<unsigned>>& g,
          const DisjointDatabase& d);

    int getHeuristic() const;
    bool canMove(Direction dir) const;
    std::vector<Direction> getMoves() const;
    std::pair<uint64_t, uint64_t> applyMove(Direction dir);
    void undoMove(const std::pair<uint64_t, uint64_t>& prev, Direction dir);

    friend std::ostream& operator<<(std::ostream& out, const Board& board);
};

#endif  // BOARD_H
