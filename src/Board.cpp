#include "../include/Board.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <unordered_map>

#include "../include/Util.h"
#include "../include/WalkingDistance.h"

int findBlank(const std::array<int, 16>& g) {
    for (int i = 0; i < 16; i++) {
        if (g[i] == 0) {
            return i;
        }
    }
}

Board::Board(const std::array<int, 16>& g, const DisjointDatabase& d)
    : blank(findBlank(g)),
      grid(g),
      database(d),
      wdRowIndex(WalkingDistance::getIndex(g)),
      wdColIndex(WalkingDistance::getIndex(g, false)) {
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

int Board::getHeuristic() const {
    return std::max(std::max(database.getHeuristic(patterns),
                             database.getHeuristic(mirrPatterns)),
                    WalkingDistance::getHeuristic(wdRowIndex) +
                        WalkingDistance::getHeuristic(wdColIndex));
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
            wdRowIndex = WalkingDistance::edge(wdRowIndex, 1, (tile - 1) / 4);
            break;
        }
        case Direction::R:
            patterns[index] -= tileDeltas[tile];
            wdColIndex = WalkingDistance::edge(wdColIndex, 0, (tile - 1) % 4);
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
            wdRowIndex = WalkingDistance::edge(wdRowIndex, 0, (tile - 1) / 4);
            break;
        }
        default:
            patterns[index] += tileDeltas[tile];
            wdColIndex = WalkingDistance::edge(wdColIndex, 1, (tile - 1) % 4);
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
