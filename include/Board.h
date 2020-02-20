#ifndef BOARD_H
#define BOARD_H

#include "Direction.h"
#include "DisjointDatabase.h"

#include <ostream>
#include <vector>

class Board {
private:
    int blank;  // Position of blank (since patterns don't store the blank)
    std::vector<int> grid;           // Value to position mapping
    std::vector<uint64_t> patterns;  // Pattern IDs
    const DisjointDatabase& database;

    std::vector<int> deltas;
    std::vector<int> tileDeltas;

    Direction reverse[4] = {Direction::D, Direction::L, Direction::U,
                            Direction::R};
    std::vector<std::vector<bool>> moveList2;

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
    std::vector<std::vector<std::vector<Direction>>> moveList;

    inline int getTile(int posn);
    inline void setTile(int posn, int tile);

public:
    const int WIDTH, HEIGHT;

    Board(const std::vector<std::vector<int>>& g, const DisjointDatabase& d);

    int getHeuristic() const;
    const std::vector<Direction>& getMoves() const;
    const std::vector<Direction>& getMoves(Direction prevMove) const;
    bool canMove(Direction dir, Direction prev) const;
    std::pair<uint64_t, int> applyMove(Direction dir);
    void undoMove(const std::pair<uint64_t, int>& prev, Direction dir);

    virtual ~Board();

    friend std::ostream& operator<<(std::ostream& out, const Board& board);
};

#endif  // BOARD_H
