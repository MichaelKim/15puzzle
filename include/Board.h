#ifndef BOARD_H
#define BOARD_H

#include "Direction.h"
#include "Point.h"

#include <ostream>
#include <vector>

using namespace std;

class Board {
    private:
        Point blank;

    public:
        vector<vector<int>> grid;

        Board(vector<vector<int>> grid);
        Board(string id, int width, int height);
        virtual ~Board();

        string getId();
        Point getBlank();
        bool canShiftBlank(Direction dir);
        void shiftBlank(Direction dir);

        friend ostream& operator<<(ostream& out, const Board& board);
};


#endif // BOARD_H
