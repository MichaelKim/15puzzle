#ifndef PARTIALDATABASE_H
#define PARTIALDATABASE_H

#include <string>
#include <vector>

#include "Pattern.h"

class PartialDatabase {
private:
    std::string filename;
    std::vector<std::vector<uint>> grid;
    const uint WIDTH, HEIGHT;
    uint size;

    void generateDists();
    void saveDists();

public:
    PartialDatabase(std::vector<std::vector<uint>> grid,
                    const std::string& dbName, int index);

    virtual ~PartialDatabase();

    std::vector<int> tiles;  // Sorted list of tiles in pattern
    std::vector<int> distMap;
};

#endif  // PARTIALDATABASE_H
