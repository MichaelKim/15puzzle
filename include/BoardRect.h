#ifndef BOARDRECT_H
#define BOARDRECT_H

#include <array>
#include <ostream>
#include <vector>

#include "Direction.h"
#include "DisjointDatabase.h"

/*
    Rectangular board
    - No mirror
    - No walking distance
*/

class BoardRect {
    const int WIDTH;
    const int HEIGHT;

    // {0, -1}, {1, 0}, {0, 1}, {-1, 0}}
    const std::array<int, 4> deltas;  // Blank deltas

    // Faster than computing on the fly
    const std::vector<std::array<bool, 4>> canMoveList;

    // Tiles
    int blank;  // Position of blank (since patterns don't store the blank)
    std::vector<int> grid;  // Value to position mapping

    // Used for disjoint database
    std::vector<uint64_t> patterns;  // Pattern IDs

    int getTile(int posn) const;
    void setTile(int posn, int tile);

    int getDelta(const std::vector<int>& g, int tile, int offset) const;

    struct MoveState {
        uint64_t pattern;
        int blank;
    };

public:
    BoardRect(const std::vector<int>& g, int width, int height);

    int getHeuristic() const;
    bool canMove(Direction dir);
    // Should be run only once at start of search
    std::vector<Direction> getMoves() const;
    MoveState applyMove(Direction dir);
    void undoMove(const MoveState& prev);

    friend std::ostream& operator<<(std::ostream& out, const BoardRect& board);
};

#endif  // BOARDRECT_H
