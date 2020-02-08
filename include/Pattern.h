#ifndef PATTERN_H
#define PATTERN_H

#include "Direction.h"
#include "Point.h"

#include <ostream>
#include <vector>

class Pattern {
private:
    std::vector<int> deltas;  // Indexed by value

    uint64_t grid;

    int perm(int n, int k) const;

    inline int getCell(int x, int y) const;
    inline void setCell(int x, int y, int n);

public:
    const int WIDTH, HEIGHT;
    std::vector<Point> tiles;
    uint64_t id;

    Pattern(std::vector<std::vector<int>> g);
    virtual ~Pattern();

    int size();
    bool canShift(int index, Direction dir);
    void shiftCell(int index, Direction dir);

    friend std::ostream& operator<<(std::ostream& out, const Pattern& pattern);
};

#endif  // PATTERN_H
