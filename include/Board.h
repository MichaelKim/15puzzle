#ifndef BOARD_H
#define BOARD_H

#include "Direction.h"
#include "Point.h"

#include <ostream>
#include <vector>

using namespace std;

class Board {
    private:

    public:
        vector<vector<int>> grid;
        vector<Point> cells;

        Board(vector<vector<int>> grid);
        virtual ~Board();

        string getId();
        bool canShift(int index, Direction dir);
        string getShiftId(int index, Direction dir);
        void shiftCell(int index, Direction dir);

        friend ostream& operator<<(ostream& out, const Board& board);
};


#endif // BOARD_H
