#include "PartialDatabase.h"

#include "Board.h"
#include "State.h"

#include <ctime>
#include <fstream>
#include <iostream>
#include <queue>

using namespace std;

PartialDatabase::PartialDatabase(vector<vector<int>> grid, string filename): board(Board(grid)), filename(filename) {
    int count = 0;
    for (int y = 0; y < grid.size(); y++) {
        for (int x = 0; x < grid[y].size(); x++) {
            if (grid[y][x] > 0) {
                cells[grid[y][x]] = count++;
            }
        }
    }
}

/* PartialDatabase::PartialDatabase(vector<vector<int>> grid, vector<int> targets) {
    int count = 0;
    for (int y = 0; y < grid.size(); y++) {
        for (int x = 0; x < grid[y].size(); x++) {
            auto it = find(targets.begin(), targets.end(), grid[y][x]);
            if (it != targets.end()) {
                cells[grid[y][x]] = count++;
            }
        }
    }
} */

void PartialDatabase::setup() {
    ifstream file("database-" + filename + ".txt");
    if (!file.good()) {
        // Database file missing, generate database
        cout << "Generating database" << endl;
        generateDists();
        saveDists();
    }
    else {
        // Read database from file
        cout << "Parsing database" << endl;
        //int width, height, numCells;
        //file >> width >> height >> numCells;
        //vector<Point> cells(numCells);
        //for (int i = 0, x, y; i < numCells; i++) {
            //file >> x >> y;
            //cells.push_back({x, y});
        //}

        string id;
        int dist;

        while (file >> id >> dist) {
            distMap[id] = dist;
        }
    }

    cout << "Number of entries: " << distMap.size() << endl;

    cout << "Cells:" << endl;
    for (auto i: cells) {
        cout << i.first << " " << i.second << endl;
    }

    cout << "Board:" << endl << board << endl;
}

void PartialDatabase::generateDists() {
    // board has 0 for irrelevant cells, non-0 for those part of the database
    int count = 0;
    int dist = 0;

    cout << "Generating database for: " << endl;
    cout << board << endl;

    clock_t startTime, test;
    double duration;

    startTime = clock();

    // Start of algorithm
    distMap.clear();
    queue<State> bfs;

    bfs.push({board, 0});
    distMap[board.getId()] = 0;

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
                if (shiftId.length() > 0 && distMap.find(shiftId) == distMap.end()) {
                    State next = {curr.board, curr.dist + 1};
                    next.board.shiftCell(i, dir);

                    distMap[shiftId] = next.dist;

                    bfs.push(next);
                }
            }
        }
    }

    duration = (clock() - startTime) / (double)CLOCKS_PER_SEC;
    cout<< "Time taken: " << duration << endl;
}

void PartialDatabase::saveDists() {
    // Store file
    ofstream file("database-" + filename + ".txt");
    if (!file.good()) {
        cerr << "Could not generate database file: database-" + filename + ".txt" << endl;
    }
    else {
        // Board dimensions (width, height)
        //file << board.grid[0].size() << " " << board.grid.size() << endl;
        // Number of cells, locations of cells (id)
        //file << board.cells.size() << " " << board.getId() << endl;
        for (auto it: distMap) {
            file << it.first << " " << it.second << endl;
        }

        file.close();
    }
}

int PartialDatabase::getDist(vector<vector<int>> grid) {
    return 0;
}

PartialDatabase::~PartialDatabase() {}
