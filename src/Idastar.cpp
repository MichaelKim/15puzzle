#include "../include/Idastar.h"

#include <future>
#include <thread>

#include "../include/Util.h"

constexpr int INF = 1000;

Idastar::Idastar() : minCost(INF), limit(0), nodes(0), found(false) {}

Direction inverse(Direction move) {
    switch (move) {
        case Direction::U:
            return Direction::D;
        case Direction::L:
            return Direction::R;
        case Direction::D:
            return Direction::U;
        case Direction::R:
            return Direction::L;
        default:
            assertm(0, "Unknown direction in inverse");
    }
}

std::vector<Direction> Idastar::solve(const Board& start) {
    // Uses IDA* with additive pattern disjoint database heuristics
    DEBUG("Running multithreaded");
    DEBUG("Solving: \n" << start);

    nodes = 1;
    limit = start.getHeuristic();

    if (limit == 0) {
        DEBUG("Already solved");
        return {};
    }

    DEBUG("Limit, Nodes:");

    // Starting moves (for prevMove)
    auto startMoves = start.getMoves();

    while (true) {
        DEBUG(" " << limit << ", " << nodes);

        std::vector<std::future<std::vector<Direction>>> checks;
        std::transform(
            startMoves.begin(), startMoves.end(), std::back_inserter(checks),
            [this, &start](const auto startDir) {
                return std::async(
                    std::launch::async,
                    [this](auto copy, auto dir) -> std::vector<Direction> {
                        std::vector<Direction> path;
                        copy.applyMove(dir);
                        DEBUG("START " << dir);

                        if (dfs(copy, 1, inverse(dir), path)) {
                            bool expected = false;
                            if (found.compare_exchange_strong(expected, true)) {
                                DEBUG("FOUND " << dir);
                                path.push_back(dir);
                                return path;
                            }
                        }

                        DEBUG("END " << dir);
                        return {};
                    },
                    start, startDir);
            });

        for (int i = 0; i < checks.size(); i++) {
            auto path = checks[i].get();
            if (!path.empty()) {
                DEBUG("Nodes expanded: " << nodes.load());
                return path;
            }
        }

        limit += 2;
    }
}

bool Idastar::dfs(Board& node, int g, Direction prevMove,
                  std::vector<Direction>& path) {
    auto h = node.getHeuristic();
    auto f = g + h;

    if (h == 0) [[unlikely]] {
            // Found goal state (heuristic = 0)
            return true;
        }
    else if (f > limit) {
        // Exceeded search depth
        // Exit early if other thread found the solution
        return found.load();
    }

    nodes++;

    for (int i = 0; i < 4; i++) {
        auto dir = static_cast<Direction>(i);
        if (prevMove != dir && node.canMove(dir)) {
            auto prev = node.applyMove(dir);

            if (dfs(node, g + 1, inverse(dir), path)) {
                path.push_back(dir);
                return true;
            }

            node.undoMove(prev);
        }
    }

    return false;
}
