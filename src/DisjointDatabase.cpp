#include "DisjointDatabase.h"

using namespace std;

DisjointDatabase::DisjointDatabase(vector<vector<vector<int>>> grids) {
    for (int i = 0; i < grids.size(); i++) {
        databases.push_back(PartialDatabase(grids[i], to_string(i)));
    }
}

void DisjointDatabase::setup() {
    for (PartialDatabase pd: databases) {
        pd.setup();
    }
}

vector<Direction> DisjointDatabase::solve(vector<vector<int>> grid) {
    // Will need to get the distances for each partial database
    return vector<Direction>{Direction::N, Direction::E};
}

DisjointDatabase::~DisjointDatabase() {}
