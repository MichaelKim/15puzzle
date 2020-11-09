#include "../include/Board.h"

#include <algorithm>
#include <execution>
#include <functional>
#include <iomanip>
#include <numeric>
#include <unordered_map>

#include "../include/Util.h"
#include "../include/WalkingDistance.h"

Board::Board(const std::vector<int>& g, int width, int height)
    : WIDTH(width),
      HEIGHT(height),
      deltas({-width, 1, width, -1}),
      canMoveList(calcMoveList(width, height)),
      blank(getBlank(g)),
      grid(g),
      mirrGrid(width * height),
      patterns(DisjointDatabase::calculatePatterns(g)),
      wdRowIndex(WalkingDistance::getIndex(g)),
      wdColIndex(WalkingDistance::getIndex(g, false)) {
    assertm((int)g.size() == width * height, "Wrong board dimensions");

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        mirrGrid[i] =
            DisjointDatabase::mirrPos[grid[DisjointDatabase::mirror[i]]];
    }
    mirrPatterns = DisjointDatabase::calculatePatterns(mirrGrid);
}

int Board::getHeuristic() const {
    return std::max(std::max(DisjointDatabase::getHeuristic(patterns),
                             DisjointDatabase::getHeuristic(mirrPatterns)),
                    WalkingDistance::costs[wdRowIndex] +
                        WalkingDistance::costs[wdColIndex]);
}

std::vector<Direction> Board::getMoves() const {
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

inline int Board::getDelta(const std::vector<int>& g, int tile,
                           int offset) const {
    // Which pattern the sliding tile is in
    const auto index = DisjointDatabase::where[tile];
    const auto delta = DisjointDatabase::tileDeltas[tile];
    return std::transform_reduce(
        std::execution::par_unseq, g.cbegin() + offset + 1,
        g.cbegin() + offset + WIDTH + 1, delta, std::plus<>(),
        [&index, &tile, &delta](const auto skip) {
            if (DisjointDatabase::where[skip] != index) {
                return delta;
            } else if (skip > tile) {
                return delta + DisjointDatabase::tileDeltas[skip];
            } else {
                return 0;
            }
        });
}

// Pattern ID, pattern index
Board::MoveState Board::applyMove(Direction dir) {
    // Position of sliding tile (and new blank)
    const auto newBlank = blank + deltas[static_cast<int>(dir)];
    // Value of sliding tile
    const auto tile = getTile(newBlank);

    // Set value of slid tile
    setTile(blank, tile);

    // Update pattern
    const auto index =
        DisjointDatabase::where[tile];  // Which pattern the sliding tile is in
    const auto oldPattern = patterns[index];  // Storing for undo

    // Update mirror grid, pattern
    const auto mirrBlank = DisjointDatabase::mirror[blank];
    const auto mirrNewBlank = DisjointDatabase::mirror[newBlank];
    const auto mirrTile = getMirrTile(mirrNewBlank);
    setMirrTile(mirrBlank, mirrTile);
    const auto mirrIndex = DisjointDatabase::where[mirrTile];
    const auto oldMirrPattern = mirrPatterns[mirrIndex];

    // Walking Distance
    const auto prevRowIndex = wdRowIndex;
    const auto prevColIndex = wdColIndex;

    switch (dir) {
        case Direction::U:
            patterns[index] += getDelta(grid, tile, newBlank);
            wdRowIndex = WalkingDistance::edgesDown[wdRowIndex]
                                                   [WalkingDistance::row[tile]];
            // Mirrored L
            mirrPatterns[mirrIndex] += DisjointDatabase::tileDeltas[mirrTile];
            break;
        case Direction::R:
            patterns[index] -= DisjointDatabase::tileDeltas[tile];
            wdColIndex = WalkingDistance::edgesUp[wdColIndex]
                                                 [WalkingDistance::col[tile]];
            // Mirrored D
            mirrPatterns[mirrIndex] -= getDelta(mirrGrid, mirrTile, mirrBlank);
            break;
        case Direction::D:
            patterns[index] -= getDelta(grid, tile, blank);
            wdRowIndex = WalkingDistance::edgesUp[wdRowIndex]
                                                 [WalkingDistance::row[tile]];
            // Mirrored R
            mirrPatterns[mirrIndex] -= DisjointDatabase::tileDeltas[mirrTile];
            break;
        case Direction::L:
            patterns[index] += DisjointDatabase::tileDeltas[tile];
            wdColIndex = WalkingDistance::edgesDown[wdColIndex]
                                                   [WalkingDistance::col[tile]];
            // Mirrored U
            mirrPatterns[mirrIndex] +=
                getDelta(mirrGrid, mirrTile, mirrNewBlank);
            break;
    }

    // Update blank tile
    const auto oldBlank = blank;
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
    int index = DisjointDatabase::where[tile];
    patterns[index] = pattern;

    // Update mirrored grid
    auto mirrBlank = DisjointDatabase::mirror[blank];
    auto mirrNewBlank = DisjointDatabase::mirror[newBlank];
    auto mirrTile = getMirrTile(mirrNewBlank);
    setMirrTile(mirrBlank, mirrTile);
    int mirrIndex = DisjointDatabase::where[mirrTile];
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
            int i = y * board.WIDTH + x;
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
