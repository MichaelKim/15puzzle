#include "../include/Board.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <unordered_map>

// Faster than computing on the fly
constexpr std::array<std::array<bool, 4>, 16> Board::initMoveList() {
    std::array<std::array<bool, 4>, 16> canMoveList = {};

    // Blank position
    for (int i = 0; i < 16; i++) {
        canMoveList[i][static_cast<int>(Direction::U)] = (i / 4) > 0;
        canMoveList[i][static_cast<int>(Direction::R)] = (i % 4) < WIDTH - 1;
        canMoveList[i][static_cast<int>(Direction::D)] = (i / 4) < HEIGHT - 1;
        canMoveList[i][static_cast<int>(Direction::L)] = (i % 4) > 0;
    }

    return canMoveList;
}

constexpr std::array<int, 16> Board::calculateMirror() {
    // Calculate mirror positions
    // TODO: test with blank not in top-left or bottom-right
    std::array<int, 16> mirror;
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        int y = i / WIDTH;
        int x = i % WIDTH;
        mirror[i] = (x * WIDTH) + y;
    }
    return mirror;
}

Board::Board(const std::array<int, 16>& g, const DisjointDatabase& d,
             const WalkingDistance& w)
    : blank(findBlank(g)),
      grid(g),
      mirror(calculateMirror()),
      database(d),
      wdDb(w),
      canMoveList(initMoveList()),
      wdRowIndex(-1),
      wdColIndex(-1) {
    // Calculate deltas
    auto numPatterns = database.numPatterns();

    // The tiles in each pattern
    std::vector<std::vector<int>> patternTiles(numPatterns);
    for (int i = 1; i < WIDTH * HEIGHT; i++) {  // Ignore blank tile (0)
        patternTiles[database.where[i]].push_back(i);
    }

    patterns = generatePatterns(grid, patternTiles);

    // Calculate tileDeltas
    tileDeltas = calculateDeltas(patternTiles);

    mirrGrid.fill(0);
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        mirrGrid[i] = database.mirrPos[grid[mirror[i]]];
    }
    mirrPatterns = generatePatterns(mirrGrid, patternTiles);

    calculateWDIndex();
}

int Board::findBlank(const std::array<int, 16>& g) {
    for (int i = 0; i < 16; i++) {
        if (g[i] == 0) {
            return i;
        }
    }
}

std::array<int, 16> Board::calculateDeltas(
    const std::vector<std::vector<int>>& patternTiles) {
    std::array<int, 16> tileDeltas;
    tileDeltas.fill(1);

    for (auto& tiles : patternTiles) {
        for (int j = tiles.size() - 2; j >= 0; j--) {
            tileDeltas[tiles[j]] =
                tileDeltas[tiles[j + 1]] * (WIDTH * HEIGHT - 1 - j);
        }
    }

    return tileDeltas;
}

std::vector<uint64_t> Board::generatePatterns(
    const std::array<int, 16>& grid,
    const std::vector<std::vector<int>>& patternTiles) {
    auto numPatterns = database.numPatterns();
    std::vector<uint64_t> pat(numPatterns, 0);

    for (int i = 0; i < numPatterns; i++) {
        auto& tiles = patternTiles[i];

        // Calculate pattern
        std::vector<unsigned> startPos(WIDTH * HEIGHT, 0);
        std::unordered_map<int, int> before;
        for (int j = 0; j < WIDTH * HEIGHT; j++) {
            if (database.where[grid[j]] == i) {
                // New tile found
                unsigned beforeCount = 0;

                // Count number of preceding pattern tiles that's smaller
                for (auto& it : before) {
                    if (it.first < grid[j]) {
                        beforeCount++;
                    }
                }

                before[grid[j]] = beforeCount;
                startPos[grid[j]] = j;
            }
        }

        int j = WIDTH * HEIGHT;
        for (auto tile : tiles) {
            pat[i] *= j--;
            pat[i] += startPos[tile] - before[tile];
        }
    }

    return pat;
}

void Board::calculateWDIndex() {
    std::array<std::array<int, 4>, 4> rowTable{};
    std::array<std::array<int, 4>, 4> colTable{};
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int tile = grid[y * WIDTH + x];
            if (tile > 0) {
                rowTable[y][(tile - 1) / 4]++;
                colTable[x][(tile - 1) % 4]++;
            }
        }
    }

    // Compress WD tables
    uint64_t rowComp = 0;
    uint64_t colComp = 0;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            rowComp = (rowComp << 3) | rowTable[y][x];
            colComp = (colComp << 3) | colTable[y][x];
        }
    }

    // Convert to index
    for (int i = 0; i < wdDb.tables.size(); i++) {
        if (wdDb.tables[i] == rowComp) {
            wdRowIndex = i;
            if (wdColIndex != -1) return;
        }
        if (wdDb.tables[i] == colComp) {
            wdColIndex = i;
            if (wdRowIndex != -1) return;
        }
    }
}

int Board::getHeuristic() const {
    return std::max(std::max(database.getHeuristic(patterns),
                             database.getHeuristic(mirrPatterns)),
                    wdDb.costs[wdRowIndex] + wdDb.costs[wdColIndex]);
}

std::vector<Direction> Board::getMoves() const {
    // Should be run only once at start of search
    if (blank < WIDTH) {           // top
        if (blank % WIDTH == 0) {  // left
            return {Direction::R, Direction::D};
        }
        if (blank % WIDTH == WIDTH - 1) {  // right
            return {Direction::D, Direction::L};
        }
        return {Direction::R, Direction::D, Direction::L};
    }
    if (blank >= (WIDTH - 1) * HEIGHT) {  // bottom
        if (blank % WIDTH == 0) {         // left
            return {Direction::U, Direction::R};
        }
        if (blank % WIDTH == WIDTH - 1) {  // right
            return {Direction::U, Direction::L};
        }
        return {Direction::U, Direction::R, Direction::L};
    }
    if (blank % WIDTH == 0) {  // left
        return {Direction::U, Direction::R, Direction::D};
    }
    if (blank % WIDTH == WIDTH - 1) {  // right
        return {Direction::U, Direction::D, Direction::L};
    }

    return {Direction::U, Direction::R, Direction::D, Direction::L};
}

inline int Board::getTile(int posn) const { return grid[posn]; }

inline void Board::setTile(int posn, int tile) { grid[posn] = tile; }

inline int Board::getMirrTile(int posn) const { return mirrGrid[posn]; }

inline void Board::setMirrTile(int posn, int tile) { mirrGrid[posn] = tile; }

bool Board::canMove(Direction dir) {
    return canMoveList[blank][static_cast<int>(dir)];
}

// Pattern ID, pattern index
Board::MoveState Board::applyMove(Direction dir) {
    // Position of blank
    const auto& delta = deltas[static_cast<int>(dir)];
    // Position of sliding tile (and new blank)
    const auto newBlank = blank + delta;
    // Value of sliding tile
    const auto tile = getTile(newBlank);

    // Set value of slid tile
    setTile(blank, tile);

    // Update pattern
    int index = database.where[tile];   // Which pattern the sliding tile is in
    auto oldPattern = patterns[index];  // Storing for undo

    auto prevRowIndex = wdRowIndex;
    auto prevColIndex = wdColIndex;
    switch (dir) {
        case Direction::U: {
            int numSkips = 1;
            int skipDelta = 0;
            for (int i = 1; i <= WIDTH; i++) {
                int skip = getTile(i + newBlank);
                if (database.where[skip] != index) {
                    numSkips++;  // Blank
                } else if (skip > tile) {
                    numSkips++;  // Greater
                    skipDelta += tileDeltas[skip];
                }
            }
            patterns[index] += skipDelta + numSkips * tileDeltas[tile];

            // Update WD
            wdRowIndex = wdDb.edges[wdRowIndex][1][(tile - 1) / 4];
            break;
        }
        case Direction::R:
            patterns[index] -= tileDeltas[tile];
            wdColIndex = wdDb.edges[wdColIndex][0][(tile - 1) % 4];
            break;
        case Direction::D: {
            int numSkips = 1;
            int skipDelta = 0;
            for (int i = 1; i <= WIDTH; i++) {
                int skip = getTile(i + blank);
                if (database.where[skip] != index) {
                    numSkips++;
                } else if (skip > tile) {
                    numSkips++;
                    skipDelta += tileDeltas[skip];
                }
            }
            patterns[index] -= skipDelta + numSkips * tileDeltas[tile];

            // Update WD
            wdRowIndex = wdDb.edges[wdRowIndex][0][(tile - 1) / 4];
            break;
        }
        default:
            patterns[index] += tileDeltas[tile];
            wdColIndex = wdDb.edges[wdColIndex][1][(tile - 1) % 4];
            break;
    }

    // Update mirror grid, pattern
    auto mirrBlank = mirror[blank];
    auto mirrNewBlank = mirror[newBlank];
    auto mirrTile = getMirrTile(mirrNewBlank);
    setMirrTile(mirrBlank, mirrTile);
    int mirrIndex = database.where[mirrTile];
    auto oldMirrPattern = mirrPatterns[mirrIndex];

    switch (dir) {
        case Direction::L: {
            int numSkips = 1;
            int skipDelta = 0;
            for (int i = 1; i <= WIDTH; i++) {
                int skip = getMirrTile(i + mirrNewBlank);
                if (database.where[skip] != mirrIndex) {
                    numSkips++;
                } else if (skip > mirrTile) {
                    numSkips++;
                    skipDelta += tileDeltas[skip];
                }
            }
            mirrPatterns[mirrIndex] +=
                skipDelta + numSkips * tileDeltas[mirrTile];
            break;
        }
        case Direction::D:
            mirrPatterns[mirrIndex] -= tileDeltas[mirrTile];
            break;
        case Direction::R: {
            int numSkips = 1;
            int skipDelta = 0;
            for (int i = 1; i <= WIDTH; i++) {
                int skip = getMirrTile(i + mirrBlank);
                if (database.where[skip] != mirrIndex) {
                    numSkips++;
                } else if (skip > mirrTile) {
                    numSkips++;
                    skipDelta += tileDeltas[skip];
                }
            }
            mirrPatterns[mirrIndex] -=
                skipDelta + numSkips * tileDeltas[mirrTile];
            break;
        }
        default:
            mirrPatterns[mirrIndex] += tileDeltas[mirrTile];
            break;
    }

    // Update blank tile
    auto oldBlank = blank;
    blank = newBlank;

    return {oldPattern, oldMirrPattern, prevRowIndex, prevColIndex, oldBlank};
}

void Board::undoMove(const Board::MoveState& prev) {
    const auto& [pattern, mirrPattern, prevRowIndex, prevColIndex, newBlank] =
        prev;

    // Value of sliding tile
    const auto tile = getTile(newBlank);

    // Set value of slid tile
    setTile(blank, tile);

    // Restore saved pattern ID
    int index = database.where[tile];
    patterns[index] = pattern;

    // Update mirrored grid
    auto mirrBlank = mirror[blank];
    auto mirrNewBlank = mirror[newBlank];
    auto mirrTile = getMirrTile(mirrNewBlank);
    setMirrTile(mirrBlank, mirrTile);
    int mirrIndex = database.where[mirrTile];
    mirrPatterns[mirrIndex] = mirrPattern;

    // Update blank tile
    blank = newBlank;

    // Update WD
    wdRowIndex = prevRowIndex;
    wdColIndex = prevColIndex;
}

std::ostream& operator<<(std::ostream& out, const Board& board) {
    for (int y = 0; y < board.HEIGHT; y++) {
        for (int x = 0; x < board.WIDTH; x++) {
            const int i = y * board.WIDTH + x;
            if (i == board.blank) {
                out << std::setw(3) << 0;
            } else {
                out << std::setw(3) << board.getTile(i);
            }
        }
        out << std::endl;
    }
    return out;
}
