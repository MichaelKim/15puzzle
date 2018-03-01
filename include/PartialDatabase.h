#ifndef PARTIALDATABASE_H
#define PARTIALDATABASE_H

#include "Board.h"
#include "Pattern.h"
#include "flat_hash_map.h"

#include <unordered_map>
#include <vector>

class PartialDatabase {
    private:
        std::string filename;
        Pattern pattern;

    public:
        std::unordered_map<int, int> cells;
        ska::flat_hash_map<uint64_t, int> distMap;

        PartialDatabase(std::vector<std::vector<int>> grid, std::string dbName, int index);
        virtual ~PartialDatabase();

        void generateDists();
        void saveDists();
};

#endif // PARTIALDATABASE_H
