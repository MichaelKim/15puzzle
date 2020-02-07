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
    ska::flat_hash_map<uint64_t, int> distMap;

    void generateDists();
    void saveDists();

public:
    std::vector<int> tiles;
    PartialDatabase(std::vector<std::vector<int>> grid, std::string dbName,
                    int index);

    int getHeuristic(const uint64_t& positions);

    virtual ~PartialDatabase();
};

#endif  // PARTIALDATABASE_H
