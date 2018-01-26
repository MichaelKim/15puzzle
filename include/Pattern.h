#ifndef PATTERN_H
#define PATTERN_H

#include "Direction.h"
#include "Point.h"

#include <ostream>
#include <vector>

using namespace std;

class Pattern {
    private:
        vector<vector<int>> grid;

    public:
        vector<Point> cells;
        
        Pattern(vector<vector<int>> grid);
        virtual ~Pattern();

        string getId();
        bool canShift(int index, Direction dir);
        string getShiftId(int index, Direction dir);
        void shiftCell(int index, Direction dir);

        friend ostream& operator<<(ostream& out, const Pattern& pattern);
};


#endif // PATTERN_H
