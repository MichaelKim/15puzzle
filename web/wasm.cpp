#include <emscripten/bind.h>
#include <vector>

#include "../include/Board.h"
#include "../include/DisjointDatabase.h"
#include "Idastar.h"

typedef std::vector<std::vector<std::vector<int>>> Vec3Int;

std::vector<int> solve(Vec3Int grids, Vec3Int boards) {
    std::vector<Board> startBoards;
    for (int i = 0; i < boards.size(); i++) {
        startBoards.push_back(Board(boards[i]));
    }

    const int WIDTH = grids[0][0].size(), HEIGHT = grids[0].size();

    DisjointDatabase* db = new DisjointDatabase(WIDTH * HEIGHT, "def", grids);
    Idastar<DisjointDatabase, Board>* search =
        new Idastar<DisjointDatabase, Board>(db);

    std::vector<Board::Move> moves = search->solve(startBoards[0]);
    std::vector<int> solution;

    for (auto& move: moves) {
        solution.push_back(static_cast<int>(move));
    }

    delete search;

    return solution;
}

EMSCRIPTEN_BINDINGS(puzzle) {
    emscripten::function("solve", &solve);
    emscripten::register_vector<int>("VecInt");
    emscripten::register_vector<std::vector<int>>("Vec2Int");
    emscripten::register_vector<std::vector<std::vector<int>>>("Vec3Int");
}
