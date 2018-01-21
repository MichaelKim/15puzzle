#ifndef PARTIALDATABASE_H
#define PARTIALDATABASE_H

#include "Board.h"

#include <unordered_map>
#include <vector>

using namespace std;

class PartialDatabase {
    private:
        string filename;
        Board board;
        unordered_map<int, int> cells;
        unordered_map<string, int> distMap;

    public:
        PartialDatabase(vector<vector<int>> grid, string filename);
        // PartialDatabase(vector<vector<int>> grid, vector<int> targets);
        virtual ~PartialDatabase();

        void setup();
        void generateDists();
        void saveDists();
        int getDist(vector<vector<int>> grid);
};

#endif // PARTIALDATABASE_H
