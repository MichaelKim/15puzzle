#ifndef PARTIALDATABASE_H
#define PARTIALDATABASE_H

#include "Pattern.h"
#include "flat_hash_map.h"

#include <unordered_map>
#include <vector>

class PartialDatabase {
private:
    std::string filename;
    Pattern pattern;

    void generateDists();
    void saveDists();

public:
    PartialDatabase(std::vector<std::vector<int>> grid, std::string dbName,
                    int index);

    virtual ~PartialDatabase();

    std::vector<int> tiles;
    ska::flat_hash_map<uint64_t, int> distMap;
};

#endif  // PARTIALDATABASE_H
