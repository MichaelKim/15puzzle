#ifndef PARTIALDATABASE_H
#define PARTIALDATABASE_H

#include "Pattern.h"
#include "flat_hash_map.h"

#include <string>
#include <vector>

class PartialDatabase {
private:
    std::string filename;
    std::vector<std::vector<int>> grid;
    const uint WIDTH, HEIGHT;
    uint size;

    void generateDists();
    void saveDists();

public:
    PartialDatabase(std::vector<std::vector<int>> grid,
                    const std::string& dbName, int index);

    virtual ~PartialDatabase();

    std::vector<int> tiles;  // Sorted list of tiles in pattern
    ska::flat_hash_map<uint64_t, int> distMap;
};

#endif  // PARTIALDATABASE_H
