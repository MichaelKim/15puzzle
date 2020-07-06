#ifndef PARTIALDATABASE_H
#define PARTIALDATABASE_H

#include <array>
#include <string>
#include <vector>

#include "Pattern.h"

class PartialDatabase {
private:
    std::string filename;
    std::array<int, 16> grid;
    const int WIDTH = 4, HEIGHT = 4;
    int size;

    void generateDists();

    void saveDists();

public:
    PartialDatabase(const std::array<int, 16>& grid, const std::string& dbName,
                    int index);

    std::vector<int> tiles;  // Sorted list of tiles in pattern
    std::vector<int> distMap;
};

#endif  // PARTIALDATABASE_H
