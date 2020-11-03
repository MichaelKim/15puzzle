#include "../include/BoardRect.h"

#include <algorithm>
#include <execution>
#include <functional>
#include <iomanip>
#include <numeric>
#include <unordered_map>

#include "../include/Util.h"
#include "../include/WalkingDistance.h"

// WIDTH < HEIGHT

BoardRect::BoardRect(const std::vector<int>& g, int width, int height)
    : WIDTH(width),
      HEIGHT(height),
      deltas({-width, 1, width, -1}),
      canMoveList(calcMoveList(width, height)),
      blank(getBlank(g)),
      grid(g),
      patterns(DisjointDatabase::calculatePatterns(g)) {}

int BoardRect::getHeuristic() const {
    return DisjointDatabase::getHeuristic(patterns);
}

std::vector<Direction> BoardRect::getMoves() const {
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

inline int BoardRect::getTile(int posn) const { return grid[posn]; }

inline void BoardRect::setTile(int posn, int tile) { grid[posn] = tile; }

bool BoardRect::canMove(Direction dir) {
    return canMoveList[blank][static_cast<int>(dir)];
}

inline int BoardRect::getDelta(const std::vector<int>& g, int tile,
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
BoardRect::MoveState BoardRect::applyMove(Direction dir) {
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

    switch (dir) {
        case Direction::U:
            patterns[index] += getDelta(grid, tile, newBlank);
            break;
        case Direction::R:
            patterns[index] -= DisjointDatabase::tileDeltas[tile];
            break;
        case Direction::D:
            patterns[index] -= getDelta(grid, tile, blank);
            break;
        case Direction::L:
            patterns[index] += DisjointDatabase::tileDeltas[tile];
            break;
    }

    // Update blank tile
    const auto oldBlank = blank;
    blank = newBlank;

    return {oldPattern, oldBlank};
}

void BoardRect::undoMove(const BoardRect::MoveState& prev) {
    const auto& [pattern, newBlank] = prev;

    // Value of sliding tile
    const auto tile = getTile(newBlank);

    // Set value of slid tile
    setTile(blank, tile);

    // Restore saved pattern ID
    int index = DisjointDatabase::where[tile];
    patterns[index] = pattern;

    // Update blank tile
    blank = newBlank;
}

std::ostream& operator<<(std::ostream& out, const BoardRect& board) {
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
