#include "../include/PartialDatabase.h"

#include "../include/Board.h"
#include "../include/Pattern.h"

#include <ctime>
#include <fstream>
#include <iostream>
#include <queue>

using namespace std;

PartialDatabase::PartialDatabase(vector<vector<int>> grid, string filename): pattern(Pattern(grid)), filename(filename) {
    int count = 0;
    for (int y = 0; y < grid.size(); y++) {
        for (int x = 0; x < grid[y].size(); x++) {
            if (grid[y][x] > 0) {
                cells[grid[y][x]] = count++;
            }
        }
    }

    ifstream file("/Users/michaelkim314/Documents/Github/15puzzle/database/database-" + filename + ".txt");
    if (!file.good()) {
        // Database file missing, generate database
        cout << "Generating database" << endl;
        generateDists();
        saveDists();
    }
    else {
        // Read database from file
        cout << "Parsing database" << endl;

        uint64_t id;
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

    cout << "Pattern:" << endl << pattern << endl;
}

void PartialDatabase::generateDists() {
    struct State {
        Pattern board;
        int dist;
    };

    // board has 0 for irrelevant cells, non-0 for those part of the database
    int count = 0;
    int dist = 0;

    cout << "Generating database for: " << endl;
    cout << pattern << endl;

    clock_t startTime;
    double duration;

    startTime = clock();

    // Start of algorithm
    distMap.clear();
    queue<State> bfs;

    bfs.push({pattern, 0});
    distMap[pattern.getId()] = 0;

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
                uint64_t shiftId = curr.board.getShiftId(i, dir);
                if (shiftId > 0 && distMap.find(shiftId) == distMap.end()) {
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
    ofstream file("/Users/michaelkim314/Documents/Github/15puzzle/database/database-" + filename + ".txt");
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

PartialDatabase::~PartialDatabase() {}
