#include "../include/Pattern.h"

#include <iomanip>
#include <iostream>
#include <ostream>
#include <vector>

using namespace std;

Pattern::Pattern(vector<vector<int>> grid): grid(grid) {
    for (int y = 0; y < grid.size(); y++) {
        for (int x = 0; x < grid[y].size(); x++) {
            if (grid[y][x] > 0) {
                cells.push_back({x, y});
            }
        }
    }
}

string Pattern::getId() {
    // string str = "";
    //
    // for (Point cell: cells) {
    //     if (str.length() > 0) {
    //         str += "_";
    //     }
    //     str += to_string(cell.x) + "_" + to_string(cell.y);
    // }
    //
    // return str;

    string id = "";
    for (int y = 0; y < grid.size(); y++) {
        for (int x = 0; x < grid[y].size(); x++) {
            if (id.length() > 0) {
                id += "_";
            }
            id += to_string(grid[y][x]);
        }
    }
    return id;
}

bool Pattern::canShift(int index, Direction dir) {
    int cellX = cells[index].x;
    int cellY = cells[index].y;

    if (dir == Direction::N) {
        return (cellY > 0 && grid[cellY - 1][cellX] == 0);
    }
    else if (dir == Direction::E) {
        return (cellX < grid[cellY].size() - 1 && grid[cellY][cellX + 1] == 0);
    }
    else if (dir == Direction::S) {
        return (cellY < grid.size() - 1 && grid[cellY + 1][cellX] == 0);
    }
    else {
        return (cellX > 0 && grid[cellY][cellX - 1] == 0);
    }
}

string Pattern::getShiftId(int index, Direction dir) {
    if (!canShift(index, dir)) return "";

    int cellX = cells[index].x;
    int cellY = cells[index].y;
    string shiftId = "";

    if (dir == Direction::N) {
        swap(grid[cellY][cellX], grid[cellY - 1][cellX]);
        shiftId = getId();
        swap(grid[cellY][cellX], grid[cellY - 1][cellX]);
    }
    else if (dir == Direction::E) {
        swap(grid[cellY][cellX], grid[cellY][cellX + 1]);
        shiftId = getId();
        swap(grid[cellY][cellX], grid[cellY][cellX + 1]);
    }
    else if (dir == Direction::S) {
        swap(grid[cellY][cellX], grid[cellY + 1][cellX]);
        shiftId = getId();
        swap(grid[cellY][cellX], grid[cellY + 1][cellX]);
    }
    else {
        swap(grid[cellY][cellX], grid[cellY][cellX - 1]);
        shiftId = getId();
        swap(grid[cellY][cellX], grid[cellY][cellX - 1]);
    }

    return shiftId;
}

// Assumes canShift(index, dir)
void Pattern::shiftCell(int index, Direction dir) {
    int cellX = cells[index].x;
    int cellY = cells[index].y;

    if (dir == Direction::N) {
        cells[index].y--;
        swap(grid[cellY][cellX], grid[cellY - 1][cellX]);
    }
    else if (dir == Direction::E) {
        cells[index].x++;
        swap(grid[cellY][cellX], grid[cellY][cellX + 1]);
    }
    else if (dir == Direction::S) {
        cells[index].y++;
        swap(grid[cellY][cellX], grid[cellY + 1][cellX]);
    }
    else {
        cells[index].x--;
        swap(grid[cellY][cellX], grid[cellY][cellX - 1]);
    }
}

Pattern::~Pattern() {}

ostream& operator<<(ostream& out, const Pattern& pattern) {
    for (vector<int> row: pattern.grid) {
        for (int i: row) {
            out << setw(3) << i;
        }
        out << endl;
    }
    return out;
}
