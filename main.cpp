#include <chrono>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "./include/Board.h"
#include "./include/DisjointDatabase.h"
#include "./include/Idastar.h"
#include "./include/InputParser.h"

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
         << endl;
}

vector<vector<vector<int>>> readDatabase(istream& input) {
    int databaseNum;
    input >> databaseNum;

    vector<vector<vector<int>>> grids(
        databaseNum,
        vector<vector<int>>(Board::SIZE, vector<int>(Board::SIZE, 0)));
    for (int i = 0; i < databaseNum; i++) {
        for (int y = 0; y < Board::SIZE; y++) {
            for (int x = 0; x < Board::SIZE; x++) {
                input >> grids[i][y][x];
            }
        }
    }
    return grids;
}

vector<Board> readBoards(istream& input) {
    int boardNum;
    input >> boardNum;

    vector<Board> boards;
    for (int i = 0; i < boardNum; i++) {
        vector<vector<int>> board(Board::SIZE, vector<int>(Board::SIZE, 0));
        for (int y = 0; y < Board::SIZE; y++) {
            for (int x = 0; x < Board::SIZE; x++) {
                input >> board[y][x];
            }
        }
        boards.push_back(Board(board));
    }
    return boards;
}

int main(int argc, const char* argv[]) {
    InputParser ip(argc, argv);

    // Help output
    if (ip.showHelp()) {
        usage();
        return 0;
    }

    // Reading database file
    string dbName = "def";
    vector<vector<vector<int>>> grids;
    if (ip.databaseExists()) {
        string dbPath = ip.getDatabase();
        dbName = dbPath.substr(dbPath.find_last_of("/") + 1);

        ifstream input = ifstream(dbPath);
        grids = readDatabase(input);
        input.close();
    }
    else {
        grids = readDatabase(cin);
    }

    // Reading board file
    vector<Board> startBoards;
    if (ip.boardExists()) {
        ifstream input = ifstream(ip.getBoard());
        startBoards = readBoards(input);
        input.close();
    }
    else {
        startBoards = readBoards(cin);
    }

    // Reg: 5, 36, 40, 52, 56, 60, 80
    // Rev: 3, 56, 68, 65

    // Setup database
    auto dbBegin = chrono::steady_clock::now();

    DisjointDatabase* db = new DisjointDatabase(dbName, grids);

    auto dbEnd = chrono::steady_clock::now();
    cout << "Database time taken: "
         << (chrono::duration_cast<chrono::microseconds>(dbEnd - dbBegin)
                 .count()) /
                1000000.0
         << endl;

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
        }
        cout << hex << b.getId() << dec << endl;
    }

    delete db;

    return 0;
}
