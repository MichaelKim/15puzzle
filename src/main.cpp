#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../include/Board.h"
#include "../include/Idastar.h"
#include "../include/InputParser.h"

using namespace std;

void usage() {
    cout << "puzzle - An optimal 15 Puzzle solver\n"
            "\n"
            "Syntax:\n"
            "    puzzle [OPTIONS]\n"
            "\n"
            "Options:\n"
            "    -b <file>\n"
            "        Board files\n"
            "    -d <file>\n"
            "        Use database file\n"
            "    -h, --help\n"
            "        Print this help\n"
            "    -i, --interactive\n"
            "        Show a playback of each solution\n"
         << endl;
}

vector<vector<vector<int>>> readDatabase(istream& input) {
    int width, height, databaseNum;
    input >> width >> height >> databaseNum;

    vector<vector<vector<int>>> grids(
        databaseNum, vector<vector<int>>(height, vector<int>(width, 0)));
    for (int i = 0; i < databaseNum; i++) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                input >> grids[i][y][x];
            }
        }
    }
    return grids;
}

vector<Board> readBoards(istream& input, const DisjointDatabase& db) {
    int width, height, boardNum;
    input >> width >> height >> boardNum;

    vector<Board> boards;
    for (int i = 0; i < boardNum; i++) {
        vector<vector<int>> board(height, vector<int>(width, 0));
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                input >> board[y][x];
            }
        }
        boards.emplace_back(board, db);
    }
    return boards;
}

int main(int argc, const char* argv[]) {
    InputParser::parse(argc, argv);

    // Help output
    if (InputParser::showHelp()) {
        usage();
        return 0;
    }

    // Reading database file
    string dbName = "def";
    vector<vector<vector<int>>> grids;
    if (InputParser::databaseExists()) {
        string dbPath = InputParser::getDatabase();
        dbName = dbPath.substr(dbPath.find_last_of('/') + 1);

        ifstream input = ifstream(dbPath);
        grids = readDatabase(input);
        input.close();
    } else {
        grids = readDatabase(cin);
    }
    if (grids.empty()) {
        cerr << "Error: must have at least one database" << endl;
        return 1;
    }

    const int WIDTH = grids[0][0].size();
    const int HEIGHT = grids[0].size();

    // Setup database
    auto dbBegin = chrono::steady_clock::now();

    DisjointDatabase db(WIDTH * HEIGHT, dbName, grids);

    auto dbEnd = chrono::steady_clock::now();
    cout << "Database time taken: "
         << (chrono::duration_cast<chrono::microseconds>(dbEnd - dbBegin)
                 .count()) /
                1000000.0
         << endl
         << endl;

    // Reading board file
    vector<Board> startBoards;
    if (InputParser::boardExists()) {
        ifstream input = ifstream(InputParser::getBoard());
        startBoards = readBoards(input, db);
        input.close();
    } else {
        startBoards = readBoards(cin, db);
    }

    // Setup search
    Idastar search;

    // Start search
    vector<vector<Direction>> answers;
    auto solveBegin = chrono::steady_clock::now();

    for (const Board& startBoard : startBoards) {
        auto singleSolveBegin = chrono::steady_clock::now();
        auto solution = search.solve(startBoard);
        auto singleSolveEnd = chrono::steady_clock::now();

        if (solution.empty()) {
            cout << "No solution found!" << endl;
        } else {
            cout << "Solution: " << solution.size() << " steps" << endl;
            for (auto dir : solution) {
                cout << dir << " ";
            }
            cout << endl;
        }

        cout << "Solve time taken: "
             << (chrono::duration_cast<chrono::microseconds>(singleSolveEnd -
                                                             singleSolveBegin)
                     .count()) /
                    1000000.0
             << endl
             << endl;

        answers.push_back(solution);
    }

    auto solveEnd = chrono::steady_clock::now();
    cout << "Total solve time taken: "
         << (chrono::duration_cast<chrono::microseconds>(solveEnd - solveBegin)
                 .count()) /
                1000000.0
         << endl;

    // Check solutions
    cout << "Checking solutions:" << endl;
    for (size_t i = 0; i < startBoards.size(); i++) {
        Board b = startBoards[i];
        auto solution = answers[i];

        for (int j = solution.size() - 1; j >= 0; j--) {
            b.applyMove(solution[j]);
            if (InputParser::showInteractive()) {
                cout << (solution.size() - j) << endl;
                cout << b << endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        }
        cout << b << endl;
    }

    return 0;
}
