#ifndef BOARD_H
#define BOARD_H

#include "Point.h"

#include <ostream>
#include <vector>

class Board {
public:
    enum class Move : int { Null, U, R, D, L };

private:
    uint64_t grid;
    Point blank;

    static const std::vector<std::vector<Move>> moves;

    inline void setCell(int x, int y, int n);
    inline int getCell(int x, int y) const;
    inline Point getBlank();

public:
    static const int MAX_LENGTH = 16;
    const int WIDTH, HEIGHT;

    Board(std::vector<std::vector<int>> g);
    virtual ~Board();

    uint64_t getId() const;
    const std::vector<Move>& getMoves(Move prevMove);
    int applyMove(Move dir);
    void undoMove(Move dir);

    friend std::ostream& operator<<(std::ostream& out, const Move& move);
    friend std::ostream& operator<<(std::ostream& out, const Board& board);
};

#endif  // BOARD_H
