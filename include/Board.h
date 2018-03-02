#ifndef BOARD_H
#define BOARD_H

#include "Point.h"

#include <ostream>
#include <vector>

class Board {
private:
    uint64_t grid;
    Point blank;

    static const std::vector<std::vector<int>> moves;

    inline void setCell(int x, int y, int n);
    inline int getCell(int x, int y) const;
    inline Point getBlank();

public:
    static const int SIZE = 4;
    static const int LEN = SIZE * SIZE;

    Board(std::vector<std::vector<int>> g);
    virtual ~Board();

    uint64_t getId() const;
    const std::vector<int>& getMoves(int prevMove);
    void applyMove(int dir);
    void undoMove(int dir);

    friend std::ostream& operator<<(std::ostream& out, const Board& board);
};

#endif  // BOARD_H
