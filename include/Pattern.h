#ifndef PATTERN_H
#define PATTERN_H

#include "Direction.h"
#include "Point.h"

#include <ostream>
#include <vector>

using namespace std;

class Pattern {
    private:
        unsigned long long int grid;

        int getCell(int x, int y) const;
        void setCell(int x, int y, int n);

    public:
        static const int SIZE = 4;

        vector<Point> cells;

        Pattern(vector<vector<int>> g);
        virtual ~Pattern();

        unsigned long long int getId();
        bool canShift(int index, Direction dir);
        unsigned long long int getShiftId(int index, Direction dir);
        void shiftCell(int index, Direction dir);

        friend ostream& operator<<(ostream& out, const Pattern& pattern);
};


#endif // PATTERN_H
