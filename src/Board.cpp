#include "../include/Board.h"

#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

Board::Board(vector<vector<int>> grid): grid(grid) {
    for (int y = 0; y < grid.size(); y++) {
        for (int x = 0; x < grid[y].size(); x++) {
            if (grid[y][x] == 0) {
                blank = {x, y};
            }
        }
    }
}

Board::Board(string id, int width, int height) {
    stringstream ss(id);

    grid = vector<vector<int>>(height, vector<int>(width, 0));

    int i;
    for (int y = 0; y < grid.size(); y++) {
        for (int x = 0; x < grid[y].size(); x++) {
            if (ss >> i) {
                grid[y][x] = i;
                if (i == 0) {
                    blank = {x, y};
                }
            }
        }
    }
}

string Board::getId() {
    string id = "";
    for (int y = 0; y < grid.size(); y++) {
        for (int x = 0; x < grid[y].size(); x++) {
            if (id.length() > 0) {
                id += " ";
            }
            id += to_string(grid[y][x]);
        }
    }
    return id;
}

Point Board::getBlank() {
    return blank;
}

bool Board::canShiftBlank(Direction dir) {
    if (dir == Direction::N) {
        return blank.y > 0;
    }
    else if (dir == Direction::E) {
        return blank.x < grid[blank.y].size() - 1;
    }
    else if (dir == Direction::S) {
        return blank.y < grid.size() - 1;
    }
    else {
        return blank.x > 0;
    }
}

void Board::shiftBlank(Direction dir) {
    if (dir == Direction::N) {
        swap(grid[blank.y][blank.x], grid[blank.y - 1][blank.x]);
        blank.y -= 1;
    }
    else if (dir == Direction::E) {
        swap(grid[blank.y][blank.x], grid[blank.y][blank.x + 1]);
        blank.x += 1;
    }
    else if (dir == Direction::S) {
        swap(grid[blank.y][blank.x], grid[blank.y + 1][blank.x]);
        blank.y += 1;
    }
    else {
        swap(grid[blank.y][blank.x], grid[blank.y][blank.x - 1]);
        blank.x -= 1;
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
