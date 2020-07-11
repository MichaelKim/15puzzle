#ifndef PARTIALDATABASE_H
#define PARTIALDATABASE_H

#include <string>
#include <vector>

#include "Pattern.h"

class PartialDatabase {
private:
    std::string filename;
    std::vector<std::vector<int>> grid;
    const int WIDTH, HEIGHT;
    int size;

    void generateDists();

    void saveDists();

public:
    PartialDatabase(const std::vector<std::vector<int>>& grid,
                    const std::string& dbName, int index);

    std::vector<int> tiles;  // Sorted list of tiles in pattern
    std::vector<int> distMap;
};

#endif  // PARTIALDATABASE_H
