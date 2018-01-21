#include "Board.h"

#include <iomanip>

using namespace std;

Board::Board(vector<vector<int>> grid): grid(grid) {
    for (int y = 0; y < grid.size(); y++) {
        for (int x = 0; x < grid[y].size(); x++) {
            if (grid[y][x] > 0) {
                cells.push_back({x, y});
            }
        }
    }
}

string Board::getId() {
    string str = "";

    for (Point cell: cells) {
        if (str.length() > 0) {
            str += "_";
        }
        str += to_string(cell.x) + "_" + to_string(cell.y);
    }

    return str;
}

bool Board::canShift(int index, Direction dir) {
    int cellX = cells[index].x;
    int cellY = cells[index].y;

    if (dir == Direction::N) {
        if (cellY > 0 && grid[cellY - 1][cellX] == 0) {
            return true;
        }
    }
    else if (dir == Direction::E) {
        if (cellX < grid[cellY].size() - 1 && grid[cellY][cellX + 1] == 0) {
            return true;
        }
    }
    else if (dir == Direction::S) {
        if (cellY < grid.size() - 1 && grid[cellY + 1][cellX] == 0) {
            return true;
        }
    }
    else {
        if (cellX > 0 && grid[cellY][cellX - 1] == 0) {
            return true;
        }
    }

    return false;
}

string Board::getShiftId(int index, Direction dir) {
    if (!canShift(index, dir)) return "";

    string shiftId = "";

    if (dir == Direction::N) {
        cells[index].y--;
        shiftId = getId();
        cells[index].y++;
    }
    else if (dir == Direction::E) {
        cells[index].x++;
        shiftId = getId();
        cells[index].x--;
    }
    else if (dir == Direction::S) {
        cells[index].y++;
        shiftId = getId();
        cells[index].y--;
    }
    else {
        cells[index].x--;
        shiftId = getId();
        cells[index].x++;
    }

    return shiftId;
}

// Assumes canShift(index, dir)
void Board::shiftCell(int index, Direction dir) {
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

Board::~Board() {}

ostream& operator<<(ostream& out, const Board& board) {
    for (vector<int> row: board.grid) {
        for (int i: row) {
            out << setw(3) << i;
        }
        out << endl;
    }
    return out;
}
