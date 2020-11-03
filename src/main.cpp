#include <algorithm>
#include <array>
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
#include "../include/Util.h"
#include "../include/WalkingDistance.h"

// Dynamic board size
// Dynamic database pattern

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

struct DBData {
    int width;
    int height;
    vector<vector<int>> grids;
};

DBData readDatabase(istream& input) {
    int width, height, databaseNum;
    input >> width >> height >> databaseNum;

    vector<vector<int>> grids(databaseNum, vector<int>(width * height, 0));
    for (auto& grid : grids) {
        for (auto& tile : grid) {
            input >> tile;
        }
    }

    return {width, height, grids};
}

vector<vector<int>> readBoards(istream& input) {
    int width, height, boardNum;
    input >> width >> height >> boardNum;

    vector<vector<int>> boards;
    for (int i = 0; i < boardNum; i++) {
        vector<int> board(width * height, 0);
        for (auto& tile : board) {
            input >> tile;
        }
        boards.push_back(board);
    }
    return boards;
}

pair<string, DBData> getDatabase() {
    if (InputParser::databaseExists()) {
        auto dbPath = InputParser::getDatabase();
        auto dbName = dbPath.substr(dbPath.find_last_of('/') + 1);

        ifstream input(dbPath);
        return {dbName, readDatabase(input)};
    }

    return {"def", readDatabase(cin)};
}

vector<vector<int>> getBoards() {
    if (InputParser::boardExists()) {
        ifstream input(InputParser::getBoard());

        return readBoards(input);
    }

    return readBoards(cin);
}

int getInversions(const vector<int>& board) {
    int inversions = 0;
    for (int i = 0; i < board.size(); i++) {
        for (int j = i + 1; j < board.size(); j++) {
            if (board[i] != 0 && board[j] != 0 && board[i] > board[j]) {
                inversions++;
            }
        }
    }
    return inversions;
}

bool solvable(const vector<int>& solution, int width, int height,
              const vector<int>& board) {
    if (width % 2 == 1 || height % 2 == 1) {
        // Odd side length
        return (getInversions(solution) % 2) == (getInversions(board) % 2);
    }

    // Even side length
    auto solutionBlankRow = getBlank(solution) / width;
    auto boardBlankRow = getBlank(board) / width;
    return getInversions(solution) % 2 !=
           (getInversions(board) % 2 == (solutionBlankRow - boardBlankRow) % 2);
}

int main(int argc, const char* argv[]) {
    InputParser::parse(argc, argv);

    // Help output
    if (InputParser::showHelp()) {
        usage();
        return 0;
    }

    // Reading database file
    const auto [dbName, dbData] = getDatabase();
    const auto [width, height, grids] = dbData;
    if (grids.empty()) {
        cerr << "Error: must have at least one database" << endl;
        return 1;
    }
    const auto solution = combine(grids);

    // Setup database
    START_TIMER(db);
    DisjointDatabase::load(grids, dbName, width, height);
    END_TIMER(db);

    // Setup WD
    START_TIMER(wd);
    WalkingDistance::load(solution, width, height);
    END_TIMER(wd);

    // Reading board file
    const auto startBoards(getBoards());

    // Setup search
    Idastar search;

    // Start search
    vector<Board> boards;
    vector<vector<Direction>> answers;
    START_TIMER(solve);
    for (const auto& startBoard : startBoards) {
        if (!solvable(solution, width, height, startBoard)) {
            cout << "No solution possible!\n";
            continue;
        }

        Board b(startBoard, width, height);

        START_TIMER(singleSolve);
        auto solution = search.solve(b);
        END_TIMER(singleSolve);

        cout << "Solution: " << solution.size() << " moves\n";
        for (auto dir : solution) {
            cout << dir << ' ';
        }
        cout << '\n';

        boards.push_back(b);
        answers.push_back(solution);
    }
    END_TIMER(solve);

    // Check solutions
    cout << "Checking solutions:" << endl;
    for (auto i = 0; i < boards.size(); i++) {
        auto board = boards[i];
        auto solution = answers[i];

        for (auto j = solution.size(); j--;) {
            board.applyMove(solution[j]);
            if (InputParser::showInteractive()) {
                cout << (solution.size() - j) << '\n';
                cout << board << '\n';
                this_thread::sleep_for(chrono::milliseconds(200));
            }
        }
        cout << board << endl;
    }

    return 0;
}
