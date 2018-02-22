#ifndef BOARD_H
#define BOARD_H

#include "Point.h"

#include <ostream>
#include <vector>

using namespace std;

class Board {
    private:
        uint64_t grid;
        Point blank;

        static const vector<vector<int>> moves;

        void setCell(int x, int y, int n);

    public:
        static const int SIZE = 4;

        Board(vector<vector<int>> g);
        virtual ~Board();

        int getCell(int x, int y) const;
        uint64_t getId() const;
        Point getBlank();
        const vector<int>& getMoves(int prevMove);
        void applyMove(int dir);
        void undoMove(int dir);

        friend ostream& operator<<(ostream& out, const Board& board);
};


#endif // BOARD_H
