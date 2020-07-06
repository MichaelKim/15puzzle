#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <ostream>
#include <vector>

#include "Direction.h"
#include "DisjointDatabase.h"

class Board {
public:
    const int WIDTH = 4, HEIGHT = 4;

private:
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

    const std::vector<std::vector<Direction>> moves = {
        /*  0 */ {Direction::U},
        /*  1 */ {Direction::R},
        /*  2 */ {Direction::D},
        /*  3 */ {Direction::L},
        /*  4 */ {Direction::U, Direction::R},
        /*  5 */ {Direction::U, Direction::D},
        /*  6 */ {Direction::U, Direction::L},
        /*  7 */ {Direction::R, Direction::D},
        /*  8 */ {Direction::R, Direction::L},
        /*  9 */ {Direction::D, Direction::L},
        /* 10 */ {Direction::U, Direction::R, Direction::D},
        /* 11 */ {Direction::U, Direction::R, Direction::L},
        /* 12 */ {Direction::U, Direction::D, Direction::L},
        /* 13 */ {Direction::R, Direction::D, Direction::L},
        /* 14 */ {Direction::U, Direction::R, Direction::D, Direction::L}};
    const std::vector<Direction>& generateMoveList(int x, int y,
                                                   Direction prevMove);
    std::array<std::vector<std::vector<Direction>>, 16> moveList;

    std::array<std::array<bool, 4>, 16> canMoveList;

    std::vector<uint64_t> generatePatterns(
        const std::array<int, 16>& grid,
        const std::vector<std::vector<int>>& patternTiles);

    int getTile(int posn) const;
    void setTile(int posn, int tile);
    int getMirrTile(int posn) const;
    void setMirrTile(int posn, int tile);

public:
    Board(const std::array<int, 16>& g, const DisjointDatabase& d);

    int getHeuristic() const;
    bool canMove(Direction dir);
    const std::vector<Direction>& getMoves() const;
    const std::vector<Direction>& getMoves(Direction prevMove) const;
    std::pair<uint64_t, uint64_t> applyMove(Direction dir);
    void undoMove(const std::pair<uint64_t, uint64_t>& prev, Direction dir);

    friend std::ostream& operator<<(std::ostream& out, const Board& board);
};

#endif  // BOARD_H
