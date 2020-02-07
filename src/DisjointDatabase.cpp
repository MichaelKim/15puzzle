#include "../include/DisjointDatabase.h"

#include "../include/Board.h"

#include <numeric>

DisjointDatabase::DisjointDatabase(
    int len, std::string name,
    std::vector<std::vector<std::vector<int>>> grids) {
    std::vector<int> where(len, -1);

    for (int i = 0; i < grids.size(); i++) {
        PartialDatabase* pd = new PartialDatabase(grids[i], name, i);
        databases.push_back(pd);

        for (auto tile : pd->tiles) {
            if (where[tile] != -1) {
                throw "Error: patterns overlapping";
            }

            where[tile] = i;
        }
    }

    // Check that the patterns perfectly cover the board
    // (except for the blank tile)
    bool foundBlank = false;
    for (auto& i : where) {
        if (i == -1) {
            if (foundBlank) {
                throw "Error: found multiple blank tiles";
            }

            // Assume blank tile
            foundBlank = true;
            i = 0;
        }
    }
}

int DisjointDatabase::getHeuristic(const Board& board) {
    uint64_t positions = board.getPositions();

    int totalDist =
        std::transform_reduce(databases.begin(), databases.end(), 0,
                              [](int a, int b) { return a + b; },
                              [positions](const auto& partial) {
                                  return partial->getHeuristic(positions);
                              });

    return totalDist;
}

DisjointDatabase::~DisjointDatabase() {
    for (PartialDatabase* pd : databases) {
        delete pd;
    }
}
