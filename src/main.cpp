#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../include/Board.h"
#include "../include/BoardRect.h"
#include "../include/DisjointDatabase.h"
#include "../include/Idastar.h"
#include "../include/InputParser.h"
#include "../include/Util.h"
#include "../include/WalkingDistance.h"

// Dynamic board size
// Dynamic database pattern

void usage() {
    std::cout << "puzzle - An optimal 15 Puzzle solver\n"
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
                 "        Show a playback of each solution\n\n";
}

struct DBData {
    int width;
    int height;
    std::vector<std::vector<int>> grids;
};

DBData readDatabase(std::istream& input) {
    int width, height, databaseNum;
    input >> width >> height >> databaseNum;

    std::vector<std::vector<int>> grids(databaseNum,
                                        std::vector<int>(width * height, 0));
    for (auto& grid : grids) {
        for (auto& tile : grid) {
            input >> tile;
        }
    }

    return {width, height, grids};
}

std::vector<std::vector<int>> readBoards(std::istream& input) {
    int width, height, boardNum;
    input >> width >> height >> boardNum;

    std::vector<std::vector<int>> boards;
    for (int i = 0; i < boardNum; i++) {
        std::vector<int> board(width * height, 0);
        for (auto& tile : board) {
            input >> tile;
        }
        boards.push_back(board);
    }
    return boards;
}

std::pair<std::string, DBData> getDatabase() {
    if (InputParser::databaseExists()) {
        auto dbPath = InputParser::getDatabase();
        auto dbName = dbPath.substr(dbPath.find_last_of('/') + 1);

        std::ifstream input(dbPath);
        return {dbName, readDatabase(input)};
    }

    return {"def", readDatabase(std::cin)};
}

std::vector<std::vector<int>> getBoards() {
    if (InputParser::boardExists()) {
        std::ifstream input(InputParser::getBoard());

        return readBoards(input);
    }

    return readBoards(std::cin);
}

int getInversions(const std::vector<int>& board) {
    int inversions = 0;
    for (std::size_t i = 0; i < board.size(); i++) {
        for (std::size_t j = i + 1; j < board.size(); j++) {
            if (board[i] != 0 && board[j] != 0 && board[i] > board[j]) {
                inversions++;
            }
        }
    }
    return inversions;
}

bool solvable(const std::vector<int>& solution, int width, int height,
              const std::vector<int>& board) {
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

template <class B>
void solve(const std::vector<int>& solution, int width, int height,
           const std::vector<std::vector<int>>& grids) {
    // Setup search
    Idastar<B> search;

    // Start search
    std::vector<std::pair<B, std::vector<Direction>>> results;
    START_TIMER(solve);
    for (const auto& startBoard : grids) {
        if (!solvable(solution, width, height, startBoard)) {
            std::cout << "No solution possible!\n";
            continue;
        }

        B board(startBoard, width, height);

        START_TIMER(singleSolve);
        auto path = search.solve(board);
        END_TIMER(singleSolve);

        std::cout << "Solution: " << path.size() << " moves\n";
        for (auto dir : path) {
            std::cout << dir << ' ';
        }
        std::cout << '\n';

        results.push_back({board, path});
    }
    END_TIMER(solve);

    // Check solutions
    std::cout << "Checking solutions:\n";
    for (auto& [board, path] : results) {
        for (auto j = path.size(); j--;) {
            board.applyMove(path[j]);
            if (InputParser::showInteractive()) {
                std::cout << (solution.size() - j) << '\n';
                std::cout << board << '\n';
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        }
        std::cout << board << '\n';
    }
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
        std::cerr << "Error: must have at least one database\n";
        return 1;
    }
    const auto solution = combine(grids);

    // Setup database
    START_TIMER(db);
    DisjointDatabase::load(grids, dbName, width, height);
    END_TIMER(db);

    if (width == height) {
        // Setup WD
        START_TIMER(wd);
        WalkingDistance::load(solution, width, height);
        END_TIMER(wd);
    }

    // Reading board file
    const auto startBoards(getBoards());

    if (width == height) {
        solve<Board>(solution, width, height, startBoards);
    } else {
        solve<BoardRect>(solution, width, height, startBoards);
    }

    return 0;
}
