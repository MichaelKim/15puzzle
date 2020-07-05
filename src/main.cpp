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
         << endl;
}

vector<vector<vector<unsigned>>> readDatabase(istream& input) {
    unsigned width, height, databaseNum;
    input >> width >> height >> databaseNum;

    vector<vector<vector<unsigned>>> grids(
        databaseNum,
        vector<vector<unsigned>>(height, vector<unsigned>(width, 0)));
    for (unsigned i = 0; i < databaseNum; i++) {
        for (unsigned y = 0; y < height; y++) {
            for (unsigned x = 0; x < width; x++) {
                input >> grids[i][y][x];
            }
        }
    }
    return grids;
}

vector<Board> readBoards(istream& input, const DisjointDatabase& db) {
    unsigned width, height, boardNum;
    input >> width >> height >> boardNum;

    vector<Board> boards;
    for (unsigned i = 0; i < boardNum; i++) {
        vector<vector<unsigned>> board(height, vector<unsigned>(width, 0));
        for (unsigned y = 0; y < height; y++) {
            for (unsigned x = 0; x < width; x++) {
                input >> board[y][x];
            }
        }
        boards.emplace_back(board, db);
    }
    return boards;
}

std::pair<std::string, vector<vector<vector<unsigned>>>> getDatabase() {
    if (InputParser::databaseExists()) {
        auto dbPath = InputParser::getDatabase();
        auto dbName = dbPath.substr(dbPath.find_last_of('/') + 1);

        ifstream input(dbPath);
        return {dbName, readDatabase(input)};
    }

    return {"def", readDatabase(cin)};
}

vector<Board> getBoards(const DisjointDatabase& db) {
    if (InputParser::boardExists()) {
        ifstream input(InputParser::getBoard());

        return readBoards(input, db);
    }

    return readBoards(cin, db);
}

void timeElapsed(decltype(chrono::steady_clock::now()) start) {
    auto end = chrono::steady_clock::now();
    cout << "Time elapsed: "
         << (chrono::duration_cast<chrono::microseconds>(end - start).count()) /
                1000000.0
         << endl
         << endl;
}

#define START_TIMER(name) auto timer_##name = chrono::steady_clock::now()
#define END_TIMER(name) timeElapsed(timer_##name)

int main(int argc, const char* argv[]) {
    InputParser::parse(argc, argv);

    // Help output
    if (InputParser::showHelp()) {
        usage();
        return 0;
    }

    // Reading database file
    const auto [dbName, grids] = getDatabase();
    if (grids.empty()) {
        cerr << "Error: must have at least one database" << endl;
        return 1;
    }

    // Setup database
    START_TIMER(db);
    DisjointDatabase db(dbName, grids);
    END_TIMER(db);

    // Reading board file
    auto startBoards(getBoards(db));

    // Setup search
    Idastar search;

    // Start search
    vector<vector<Direction>> answers;
    START_TIMER(solve);
    for (const auto& startBoard : startBoards) {
        START_TIMER(singleSolve);
        auto solution = search.solve(startBoard);
        END_TIMER(singleSolve);

        if (solution.empty()) {
            cout << "No solution found!" << endl;
        } else {
            cout << "Solution: " << solution.size() << " steps" << endl;
            for (auto dir : solution) {
                cout << dir << " ";
            }
            cout << endl;
        }

        answers.push_back(solution);
    }
    END_TIMER(solve);

    // Check solutions
    cout << "Checking solutions:" << endl;
    for (auto i = 0; i < startBoards.size(); i++) {
        auto b = startBoards[i];
        auto solution = answers[i];

        for (auto j = solution.size(); j--;) {
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
