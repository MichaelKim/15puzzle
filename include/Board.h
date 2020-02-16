#ifndef BOARD_H
#define BOARD_H

#include "DisjointDatabase.h"

#include <ostream>
#include <vector>

class Board {
public:
    enum class Move : int { Null, U, R, D, L };

private:
    int blank;      // Position of blank (since patterns don't store the blank)
    uint64_t grid;  // Value to position mapping
    std::vector<uint64_t>
        partialPositions;  // Position mapping for each pattern
    const DisjointDatabase& database;

    std::vector<int> deltas;

    const std::vector<std::vector<Move>> moves = {
        /*  0 */ {Move::U},
        /*  1 */ {Move::R},
        /*  2 */ {Move::D},
        /*  3 */ {Move::L},
        /*  4 */ {Move::U, Move::R},
        /*  5 */ {Move::U, Move::D},
        /*  6 */ {Move::U, Move::L},
        /*  7 */ {Move::R, Move::D},
        /*  8 */ {Move::R, Move::L},
        /*  9 */ {Move::D, Move::L},
        /* 10 */ {Move::U, Move::R, Move::D},
        /* 11 */ {Move::U, Move::R, Move::L},
        /* 12 */ {Move::U, Move::D, Move::L},
        /* 13 */ {Move::R, Move::D, Move::L},
        /* 14 */ {Move::U, Move::R, Move::D, Move::L}};
    const std::vector<Move>& generateMoveList(int x, int y, Move prevMove);
    std::vector<std::vector<std::vector<Move>>> moveList;

public:
    const int WIDTH, HEIGHT;

    Board(std::vector<std::vector<int>> g, const DisjointDatabase& d);

    int getHeuristic() const;
    const std::vector<Move>& getMoves(Move prevMove);
    int applyMove(Move dir);
    void undoMove(Move dir);

    virtual ~Board();

    friend std::ostream& operator<<(std::ostream& out, const Move& move);
    friend std::ostream& operator<<(std::ostream& out, const Board& board);
};

#endif  // BOARD_H
