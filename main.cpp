#include <iostream>
#include <vector>
#include <chrono>

#include "../include/DisjointDatabase.h"
#include "../include/Idastar.h"

using namespace std;

int main() {

    auto dbBegin = std::chrono::steady_clock::now();

    DisjointDatabase* db = new DisjointDatabase({{
         {1, 2, 3, 0},
         {5, 6, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}
    },{
         {0, 0, 0, 4},
         {0, 0, 7, 8},
         {0, 0, 11, 12},
         {0, 0, 0, 0}
    },{
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {9, 10, 0, 0},
         {13, 14, 15, 0}
    }});

    // DisjointDatabase* db = new DisjointDatabase({{
    //       {0, 2, 3, 4},
    //       {0, 0, 0, 0},
    //       {0, 0, 0, 0},
    //       {0, 0, 0, 0}
    //   },{
    //       {1, 0, 0, 0},
    //       {5, 6, 0, 0},
    //       {9, 10, 0, 0},
    //       {13, 0, 0, 0}
    //   },{
    //       {0, 0, 0, 0},
    //       {0, 0, 7, 8},
    //       {0, 0, 11, 12},
    //       {0, 0, 14, 15}
    //   }
    // });

    auto dbEnd = std::chrono::steady_clock::now();
    cout << "Database time taken: " << (chrono::duration_cast<chrono::microseconds>(dbEnd - dbBegin).count()) / 1000000.0 << endl;

    Idastar* search = new Idastar(db);

    vector<Board> startBoards {
        Board ({ // Solution: 5
            {1, 2, 3, 4},
            {5, 6, 8, 12},
            {9, 10, 7, 0},
            {13, 14, 11, 15}
        }),
        Board({ // Solution: 36
            { 1,  9, 11,  4},
            {14,  8,  2,  7},
            {10,  6,  3, 12},
            { 5,  0, 13, 15}
        }),
        Board({ // Solution: 40
            {2, 7, 11, 5},
            {13, 0, 9, 4},
            {14, 1, 8, 6},
            {10, 3, 12, 15}
        }),
        Board({ // Solution: 52
            {15, 14, 1, 6},
            {9, 11, 4, 12},
            {0, 10, 7, 3},
            {13, 8, 5, 2}
        }),
        Board({ // Solution: 56
            {5, 6, 13, 4},
            {10, 0, 12, 14},
            {11, 8, 9, 2},
            {15, 7, 3, 1}
        })
        // Board({
        //     { 0, 12,  9,  13},
        //     {15, 11, 10, 14},
        //     { 7,  8,  5,  6},
        //     { 4,  3,  2,  1}
        // })
    };

    vector<vector<int>> answers;

    auto solveBegin = std::chrono::steady_clock::now();

    for (Board& startBoard: startBoards) {
        vector<int> solution = search->solve(startBoard);

        if (solution.size() == 0) {
            cout << "No solution found!" << endl;
        }
        else {
            cout << "Solution: " << solution.size() << " steps" << endl;
            for (int dir: solution) {
                cout << dir << " ";
            }
            cout << endl;
        }

        answers.push_back(solution);
    }

    auto solveEnd = std::chrono::steady_clock::now();
    cout << "Total solve time taken: " << (chrono::duration_cast<chrono::microseconds>(solveEnd - solveBegin).count()) / 1000000.0 << endl;

    cout << "Checking solutions:" << endl;
    for (int i = 0; i < startBoards.size(); i++) {
        Board& b = startBoards[i];
        vector<int>& solution = answers[i];

        for (int j = solution.size() - 1; j >= 0; j--) {
            b.applyMove(solution[j]);
        }
        cout << hex << b.getId() << dec << endl;
    }

    delete db;

    return 0;
}
