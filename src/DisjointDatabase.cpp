#include "../include/DisjointDatabase.h"

#include "../include/Board.h"

#include <iostream>

#define INF 1000000

using namespace std;

DisjointDatabase::DisjointDatabase(vector<vector<vector<int>>> grids):
    numDatabases(grids.size()),
    ids(vector<uint64_t>(numDatabases, 0))
{
    where = vector<int>(Board::LEN, -1);

    for (int i = 0; i < grids.size(); i++) {
        PartialDatabase* pd = new PartialDatabase(grids[i], to_string(i));
        databases.push_back(pd);

        for (int j = 0; j < Board::LEN; j++) {
            if (pd->cells.find(j) != pd->cells.end()) {
                where[j] = i;
            }
        }
    }
}

template <size_t N> struct uint_{ };
template <size_t N, typename Lambda, typename IterT>
inline void unroller(const Lambda& f, const IterT& iter, uint_<N>) {
    unroller(f, iter, uint_<N-1>());
    f(iter + N);
}

template <typename Lambda, typename IterT>
inline void unroller(const Lambda& f, const IterT& iter, uint_<0>) {
	 f(iter);
}

int DisjointDatabase::getHeuristic(const Board& board) {
    fill(ids.begin(), ids.end(), 0);
    uint64_t temp = board.getId();

    unroller([&](const int& i) {
        uint64_t n = (temp & (0xfull << (4 * i))) >> (4 * i);

        int j = where[n];
        if (j >= 0 && j < numDatabases) {
            ids[j] |= n << (4 * (Board::LEN - i - 1));
        }
    }, 0, uint_<Board::LEN - 1>());

    int totalDist = 0;
    for (int i = 0; i < numDatabases; i++) {
        totalDist += databases[i]->distMap[ids[i]];
    }

    return totalDist;
}

DisjointDatabase::~DisjointDatabase() {
    for (PartialDatabase* pd: databases) {
        delete pd;
    }
}
