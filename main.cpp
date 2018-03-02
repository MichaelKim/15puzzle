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

int main(int argc, const char* argv[]) {
    InputParser ip(argc, argv);

    // Help output
    if (ip.showHelp()) {
        usage();
        return 0;
    }

    // Reading database file
    istream* input = &cin;
    ifstream in;
    string dbName = "def";

    if (ip.databaseExists()) {
        string arg = ip.getDatabase();
        in = ifstream(arg);

        dbName = arg.substr(arg.find_last_of("/") + 1);
        input = &in;
    }

    int databaseNum;
    *input >> databaseNum;

    vector<vector<vector<int>>> grids(
        databaseNum,
        vector<vector<int>>(Board::SIZE, vector<int>(Board::SIZE, 0)));
    for (int i = 0; i < databaseNum; i++) {
        for (int y = 0; y < Board::SIZE; y++) {
            for (int x = 0; x < Board::SIZE; x++) {
                *input >> grids[i][y][x];
            }
        }
    }
    in.close();
    input = &cin;

    // Reading board file
    if (ip.boardExists()) {
        in = ifstream(ip.getBoard());
        input = &in;
    }

    int boardNum;
    *input >> boardNum;

    vector<Board> startBoards;
    for (int i = 0; i < boardNum; i++) {
        vector<vector<int>> board (Board::SIZE, vector<int>(Board::SIZE, 0));
        for (int y = 0; y < Board::SIZE; y++) {
            for (int x = 0; x < Board::SIZE; x++) {
                *input >> board[y][x];
            }
        }
        startBoards.push_back(Board(board));
    }
    in.close();
    input = &in;

    // Reg: 5, 36, 40, 52, 56, 60, 80
    // Rev: 3, 56, 68, 65

    // Start solving
    auto dbBegin = chrono::steady_clock::now();

    DisjointDatabase* db = new DisjointDatabase(dbName, grids);

    auto dbEnd = chrono::steady_clock::now();
    cout << "Database time taken: " << (chrono::duration_cast<chrono::microseconds>(dbEnd - dbBegin).count()) / 1000000.0 << endl;

    Idastar<DisjointDatabase, Board>* search = new Idastar<DisjointDatabase, Board>(db);

    vector<vector<int>> answers;

    auto solveBegin = chrono::steady_clock::now();

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

    auto solveEnd = chrono::steady_clock::now();
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
