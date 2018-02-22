#ifndef PARTIALDATABASE_H
#define PARTIALDATABASE_H

#include "Board.h"
#include "Pattern.h"

#include <unordered_map>
#include <vector>

using namespace std;

class PartialDatabase {
    private:
        string filename;
        Pattern pattern;

    public:
        unordered_map<int, int> cells;
        unordered_map<uint64_t, int> distMap;

        PartialDatabase(vector<vector<int>> grid, string filename);
        virtual ~PartialDatabase();

        void generateDists();
        void saveDists();
};

#endif // PARTIALDATABASE_H
