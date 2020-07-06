#include "../include/Board.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <unordered_map>

constexpr std::array<std::array<bool, 4>, 16> Board::initMoveList() {
    std::array<std::array<bool, 4>, 16> canMoveList = {};

    // Blank position
    for (int i = 0; i < 16; i++) {
        // Direction
        for (int j = 0; j < 4; j++) {
            auto dir = static_cast<Direction>(j);
            switch (dir) {
                case Direction::U:
                    canMoveList[i][j] = (i / 4) > 0;
                    break;
                case Direction::R:
                    canMoveList[i][j] = (i % 4) < WIDTH - 1;
                    break;
                case Direction::D:
                    canMoveList[i][j] = (i / 4) < HEIGHT - 1;
                    break;
                default:
                    canMoveList[i][j] = (i % 4) > 0;
                    break;
            }
        }
    }

    return canMoveList;
}

Board::Board(const std::array<int, 16>& g, const DisjointDatabase& d)
    : blank(0), grid(g), database(d), canMoveList(initMoveList()) {
    for (int i = 0; i < 16; i++) {
        if (g[i] == 0) blank = i;
    }

    // Calculate deltas
    auto numPatterns = database.numPatterns();

    // The tiles in each pattern
    std::vector<std::vector<int>> patternTiles(numPatterns);
    for (int i = 1; i < WIDTH * HEIGHT; i++) {  // Ignore blank tile (0)
        patternTiles[database.where[i]].push_back(i);
    }

    patterns = generatePatterns(grid, patternTiles);

    // Calculate tileDeltas
    tileDeltas.fill(1);
    for (int i = 0; i < numPatterns; i++) {
        auto& tiles = patternTiles[i];
        for (int j = tiles.size() - 2; j >= 0; j--) {
            tileDeltas[tiles[j]] =
                tileDeltas[tiles[j + 1]] * (WIDTH * HEIGHT - 1 - j);
        }
    }

    // Calculate mirror positions
    // TODO: test with blank not in top-left or bottom-right
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        int y = i / WIDTH;
        int x = i % WIDTH;
        mirror[i] = (x * WIDTH) + y;
    }

    mirrGrid.fill(0);
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        mirrGrid[i] = database.mirrPos[grid[mirror[i]]];
    }
    mirrPatterns = generatePatterns(mirrGrid, patternTiles);
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

int Board::getHeuristic() const {
    return std::max(database.getHeuristic(patterns),
                    database.getHeuristic(mirrPatterns));
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
std::pair<uint64_t, uint64_t> Board::applyMove(Direction dir) {
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
            break;
        }
        case Direction::R:
            patterns[index] -= tileDeltas[tile];
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
            break;
        }
        default:
            patterns[index] += tileDeltas[tile];
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
    blank = newBlank;

    return {oldPattern, oldMirrPattern};
}

void Board::undoMove(const std::pair<uint64_t, uint64_t>& prev, Direction dir) {
    const auto& [pattern, mirrPattern] = prev;

    // Position of blank
    const auto& delta = deltas[static_cast<int>(dir)];
    // Position of sliding tile
    const auto newBlank = blank - delta;
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
