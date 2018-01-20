#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <iomanip>
#include <ctime>
#include <fstream>

using namespace std;

typedef unordered_map<string, int> Database;

enum class Direction {N, E, S, W};

struct Point {
    int x, y;
};

struct Board {
    vector<vector<int>> grid;
    vector<Point> cells;

    Board(vector<vector<int>> grid): grid(grid) {
        for (int y = 0; y < grid.size(); y++) {
            for (int x = 0; x < grid[y].size(); x++) {
                if (grid[y][x] > 0) {
                    cells.push_back({x, y});
                }
            }
        }
    }

    string getId() {
        string str = "";

        for (Point cell: cells) {
            if (str.length() > 0) {
                str += " ";
            }
            str += to_string(cell.x) + " " + to_string(cell.y);
        }

        return str;
    }

    bool canShift(int index, Direction dir) {
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

    string getShiftId(int index, Direction dir) {
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
    void shiftCell(int index, Direction dir) {
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
};

ostream& operator<<(ostream& out, const Board& board) {
    for (vector<int> row: board.grid) {
        for (int i: row) {
            out << setw(3) << i;
        }
        out << endl;
    }
    return out;
}

struct State {
    Board board;
    int dist;
};

void generateDatabase(vector<vector<int>> grid, string filename) {
    // board has 0 for irrelevant cells, non-0 for those part of the database
    int count = 0;
    int dist = 0;

    clock_t startTime, test;
    double duration;

    startTime = clock();

    // Start of algorithm
    Database database;
    queue<State> bfs;
    Board start(grid);

    cout << "Generating database for: " << endl;
    cout << start << endl;

    bfs.push({start, 0});
    database[start.getId()] = 0;

    while(!bfs.empty()) {
        State curr = bfs.front();
        bfs.pop();

        // Logging
        if (curr.dist > dist) {
            cout << dist << ": " << count << endl;
            dist = curr.dist;
            count = 1;
        }
        else count++;

        for (int i = 0; i < curr.board.cells.size(); i++) {
            for (int j = 0; j < 4; j++) {
                Direction dir = static_cast<Direction>(j);
                string shiftId = curr.board.getShiftId(i, dir);
                if (shiftId.length() > 0 && database.find(shiftId) == database.end()) {
                    State next = {curr.board, curr.dist + 1};
                    next.board.shiftCell(i, dir);

                    database[shiftId] = next.dist;

                    bfs.push(next);
                }
            }
        }
    }

    duration = (clock() - startTime) / (double)CLOCKS_PER_SEC;
    cout<< "Time taken: " << duration << endl;

    cout << "Size of database: " << database.size() << endl;

    // Store file
    ofstream file("database-" + filename + ".txt");
    if (!file.good()) {
        cerr << "Could not generate database file: database-" + filename + ".txt" << endl;
        return;
    }

    for (auto it: database) {
        file << it.first << " " << it.second << endl;
    }

    file.close();
}


int main() {
    cout << "Generating a disjoint pattern database (5-5-5):" << endl;

    cout << "Generating 5 tile database (top-left):" << endl;
    generateDatabase({
             {1, 2, 3, 0},
             {5, 6, 0, 0},
             {0, 0, 0, 0},
             {0, 0, 0, 0}
    }, "5a2");

    /*cout << "Generating 5 tile database (right):" << endl;
    generateDatabase({
             {0, 0, 0, 4},
             {0, 0, 7, 8},
             {0, 0, 11, 12},
             {0, 0, 0, -1}
    }, "5b");*/

    /*cout << "Generating 5 tile database (bottom):" << endl;
    generateDatabase({
             {0, 0, 0, 0},
             {0, 0, 0, 0},
             {9, 10, 0, 0},
             {13, 14, 15, -1}
    }, "5c");*/
    return 0;
}
