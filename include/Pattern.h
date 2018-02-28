#ifndef PATTERN_H
#define PATTERN_H

#include "Direction.h"
#include "Point.h"

#include <ostream>
#include <vector>

class Pattern {
    private:
        uint64_t grid;

        int getCell(int x, int y) const;
        void setCell(int x, int y, int n);

    public:
        static const int SIZE = 4;

        std::vector<Point> cells;

        Pattern(std::vector<std::vector<int>> g);
        virtual ~Pattern();

        uint64_t getId();
        bool canShift(int index, Direction dir);
        uint64_t getShiftId(int index, Direction dir);
        void shiftCell(int index, Direction dir);

        friend std::ostream& operator<<(std::ostream& out, const Pattern& pattern);
};


#endif // PATTERN_H
