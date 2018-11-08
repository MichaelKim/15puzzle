#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../include/Board.h"
#include "../include/DisjointDatabase.h"
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
            "    -p, --parallel\n"
            "        Run multithreaded IDA* (experimental)"
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

vector<Board> readBoards(istream& input) {
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
        boards.push_back(Board(board));
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
        dbName = dbPath.substr(dbPath.find_last_of("/") + 1);

        ifstream input = ifstream(dbPath);
        grids = readDatabase(input);
        input.close();
    }
    else {
        grids = readDatabase(cin);
    }
    if (grids.size() == 0) {
        cout << "Error: must have at least one database" << endl;
        return 1;
    }
    else if (grids.size() > DisjointDatabase::MAX_DATABASE) {
        cout << "Error: maximum number of databases is "
             << DisjointDatabase::MAX_DATABASE << endl;
        return 1;
    }

    const int WIDTH = grids[0][0].size(), HEIGHT = grids[0].size();

    // Reading board file
    vector<Board> startBoards;
    if (InputParser::boardExists()) {
        ifstream input = ifstream(InputParser::getBoard());
        startBoards = readBoards(input);
        input.close();
    }
    else {
        startBoards = readBoards(cin);
    }

    // Setup database
    auto dbBegin = chrono::steady_clock::now();

    DisjointDatabase* db = new DisjointDatabase(WIDTH * HEIGHT, dbName, grids);

    auto dbEnd = chrono::steady_clock::now();
    cout << "Database time taken: "
         << (chrono::duration_cast<chrono::microseconds>(dbEnd - dbBegin)
                 .count()) /
                1000000.0
         << endl << endl;

    // Setup search
    Idastar<DisjointDatabase, Board>* search =
        new Idastar<DisjointDatabase, Board>(db);

    vector<vector<Board::Move>> answers;

    // Start search
    auto solveBegin = chrono::steady_clock::now();

    for (const Board& startBoard : startBoards) {
        vector<Board::Move> solution = search->solve(startBoard);

        if (solution.size() == 0) {
            cout << "No solution found!" << endl;
        }
        else {
            cout << "Solution: " << solution.size() << " steps" << endl;
            for (auto dir : solution) {
                cout << dir << " ";
            }
            cout << endl;
        }

        auto singleSolveEnd = chrono::steady_clock::now();
        cout << "Solve time taken: "
             << (chrono::duration_cast<chrono::microseconds>(singleSolveEnd - solveBegin)
                    .count()) /
                    1000000.0
             << endl << endl;

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
    for (int i = 0; i < startBoards.size(); i++) {
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
        cout << hex << b.getId() << dec << endl;
    }

    delete db;

    return 0;
}
